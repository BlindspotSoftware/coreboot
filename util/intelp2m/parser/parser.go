package parser

import (
	"bufio"
	"fmt"
	"os"
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser/template"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

type EntryType int

const (
	EntryEmpty EntryType = iota
	EntryPad
	EntryGroup
	EntryReserved
)

// Parser entry
// ID        : pad id string
// Function  : the string that means the pad function
// DW0       : DW0 register struct
// DW1       : DW1 register struct
// 0wnership : host software ownership
type Entry struct {
	EType     EntryType
	ID        string
	Function  string
	DW0       uint32
	DW1       uint32
	Ownership uint8
}

func (e *Entry) ToMacro() []string {
	platform := platforms.GetSpecificInterface()
	line := platform.GenMacro(e.ID, e.DW0, e.DW1, e.Ownership)
	slices := strings.Split(line, "\n")
	return slices
}

// extractPad() extracts pad information from a string
func extractPad(line string) (Entry, error) {
	function, id, dw0, dw1, err := template.Apply(line)
	if err != nil {
		logs.Errorf("%v", err)
		return Entry{EType: EntryEmpty}, err
	}

	pad := Entry{
		EType:     EntryPad,
		Function:  function,
		ID:        id,
		DW0:       dw0,
		DW1:       dw1,
		Ownership: 0,
	}

	if dw0 == bits.All32 {
		pad.EType = EntryReserved
	}

	return pad, nil
}

// extractGroup() extracts information about the pad group from the string
func extractGroup(line string) Entry {
	group := Entry{
		EType:    EntryGroup,
		Function: line,
	}
	return group
}

// Extract() extracts pad information from a string
func Extract(line string, platform platforms.SpecificIf) Entry {
	if included, _ := common.KeywordsCheck(line, "GPIO Community", "GPIO Group"); included {
		return extractGroup(line)
	}

	if platform.KeywordCheck(line) {
		pad, err := extractPad(line)
		if err != nil {
			logs.Errorf("extract pad info from %s: %v", line, err)
			return Entry{EType: EntryEmpty}
		}
		return pad
	}
	logs.Infof("skip line <%s>", line)
	return Entry{EType: EntryEmpty}
}

// Run() starts the file parsing process
func Run() ([]Entry, error) {
	entries := make([]Entry, 0)

	platform := platforms.GetSpecificInterface()
	if platform == nil {
		return nil, fmt.Errorf("unknown platform")
	}

	file, err := os.Open(p2m.Config.InputPath)
	if err != nil {
		err = fmt.Errorf("input file error: %v", err)
		logs.Errorf("%v", err)
		return nil, err
	}
	defer file.Close()

	logs.Infof("parse %s file", p2m.Config.InputPath)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		entry := Extract(line, platform)
		if entry.EType != EntryEmpty {
			entries = append(entries, entry)
		}
	}

	logs.Infof("successfully completed: %d entries", len(entries))
	return entries, nil
}

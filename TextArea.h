#pragma once

#include <vector>
#include <string>

union CharPos {
	struct { size_t col, row; };
	struct { size_t x, y; };

	bool operator==(const CharPos& other) const { return col == other.col && row == other.row; }
	bool operator!=(const CharPos& other) const { return !(*this == other); }
	bool operator< (const CharPos& other) const { return (row < other.row) || (row == other.row && col < other.col); }
	bool operator> (const CharPos& other) const { return other < *this; }
	bool operator<=(const CharPos& other) const { return !(*this > other); }
	bool operator>=(const CharPos& other) const { return !(*this < other); }

	template<typename T>
	T convert() const {
		return T(x, y);
	}
};


class TextArea {
public:
	TextArea() = default;
	TextArea(std::string initial_text);

	void moveCursorUp(bool select = false);
	void moveCursorDown(bool select = false);
	void moveCursorLeft(bool select = false);
	void moveCursorRight(bool select = false);
	void moveCursorTo(CharPos to, bool select = false);

	void end(bool select = false);
	void home(bool select = false);

	void type(char c);
	void paste(std::string text);

	void backspace();
	void del();

	void selectAll();

	char getChar(CharPos at) const;
	bool isCharSelected(CharPos at) const;

	std::string getLine(size_t n) const;
	std::string getFull() const;
	std::string getSelected() const;

	size_t lineCount() const;

	CharPos getRealCursor() const;
	CharPos getVirtualCursor() const;

private:
	void insertChar(char c);

	void clampCursorToLine();
	CharPos getCharPosClamped(CharPos pos) const;

	void updateSelection(bool select);
	void deleteSelection();

	std::vector<std::string> lines{ "" };
	CharPos cursor{ 0, 0 };

	bool isSelectionMode = false;
	CharPos selectionStartCharPos = cursor;

	const int tabSize = 4;
};

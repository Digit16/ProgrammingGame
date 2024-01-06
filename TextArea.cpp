#include <vector>
#include <string>
#include <algorithm>

#include <iostream>
#include "TextArea.h"

TextArea::TextArea(std::string initialText) {
	paste(initialText);
}

void TextArea::moveCursorUp(bool select) {
	updateSelection(select);

	if (cursor.row > 0) {
		--cursor.row;
	} else {
		cursor.col = 0;
	}
}

void TextArea::moveCursorDown(bool select) {
	updateSelection(select);

	if (cursor.row + 1 < lines.size()) {
		++cursor.row;
	} else {
		cursor.col = lines[cursor.row].length();
	}
}

void TextArea::moveCursorLeft(bool select) {
	updateSelection(select);

	clampCursorToLine();
	if (cursor.col > 0) {
		--cursor.col;
	} else if (cursor.row > 0) {
		--cursor.row;
		cursor.col = lines[cursor.row].length();
	}
}

void TextArea::moveCursorRight(bool select) {
	if (isSelectionMode && !select) {
		cursor = std::max(cursor, selectionStartCharPos);
		isSelectionMode = false;
		return;
	}
	
	updateSelection(select);

	if (cursor.col < lines[cursor.row].length()) {
		++cursor.col;
	} else if (cursor.row + 1 < lines.size()) {
		++cursor.row;
		cursor.col = 0;
	}
}

void TextArea::moveCursorTo(CharPos to, bool select) {
	updateSelection(select);

	if (to.row >= lines.size()) {
		cursor.row = lines.size() - 1;
		cursor.col = lines[cursor.row].length();
	} else {
		cursor = to;
		clampCursorToLine();
	}
}


void TextArea::end(bool select) {
	updateSelection(select);

	cursor.col = lines[cursor.row].length();
}

void TextArea::home(bool select) {
	updateSelection(select);

	cursor.col = 0;
}

void TextArea::type(char c) {
	if (isSelectionMode) {
		deleteSelection();
	}

	insertChar(c);
}
	
void TextArea::paste(std::string text) {
	if (isSelectionMode) {
		deleteSelection();
	}

	for (const char& c : text) {
		insertChar(c);
	}
}

void TextArea::backspace() {
	if (isSelectionMode) {
		deleteSelection();
		return;
	}
	
	clampCursorToLine();
	if (cursor.col > 0) {
		lines[cursor.row].erase(cursor.col - 1, 1);
		--cursor.col;
	} else if (cursor.row > 0) {
		--cursor.row;
		cursor.col = lines[cursor.row].length();
		lines[cursor.row] += lines[cursor.row + 1];
		lines.erase(lines.begin() + cursor.row + 1);
	}
}

void TextArea::del() {
	if (isSelectionMode) {
		deleteSelection();
		return;
	}

	clampCursorToLine();
	if (cursor.col < lines[cursor.row].length()) {
		lines[cursor.row].erase(cursor.col, 1);
	} else if (cursor.row + 1 < lines.size()) {
		lines[cursor.row] += lines[cursor.row + 1];
		lines.erase(lines.begin() + cursor.row + 1);
	}
}

void TextArea::selectAll() {
	cursor = { 0, 0 };
	selectionStartCharPos = { lines[lines.size() - 1].length(), lines.size() - 1 };
	isSelectionMode = true;
}

char TextArea::getChar(CharPos at) const {
	if (at.row < lines.size() && at.col < lines[at.row].length() ) {
		return lines[at.row][at.col];
	}
	return 0;
}

bool TextArea::isCharSelected(CharPos at) const {
	if (!isSelectionMode) return false;

	bool between = selectionStartCharPos <= at && at <= cursor || cursor <= at && at < selectionStartCharPos;
	bool filled = at.col <= getLine(at.row).length();

	return between && filled;
}

std::string TextArea::getLine(size_t n) const {
	if (n < lines.size()) {
		return lines[n];
	}
	return "";
}

std::string TextArea::getFull() const {
	std::string rv = "";
	for (const std::string& line : lines) {
		rv += line + '\n';
	}

	if (!rv.empty()) {
		rv.pop_back();
	}

	return rv;
}

std::string TextArea::getSelected() const {
	if (!isSelectionMode) return "";

	CharPos start = std::min(cursor, selectionStartCharPos);
	CharPos end = std::max(cursor, selectionStartCharPos);

	if (start.row == end.row) {
		return lines[start.row].substr(start.col, end.col - start.col);
	} else {
		std::string rv = "";
		rv += lines[start.row].substr(start.col) + '\n';
		for (size_t i = start.row + 1; i < end.row; ++i) {
			rv += lines[i] + '\n';
		}
		rv += lines[end.row].substr(0, end.col);
		return rv;
	}
}

size_t TextArea::lineCount() const {
	return lines.size();
}

CharPos TextArea::getRealCursor() const {
	return getCharPosClamped(cursor);
}

CharPos TextArea::getVirtualCursor() const {
	return cursor;
}

void TextArea::insertChar(char c) {
	if (c == '\r') return;
	if (c == '\0') return;

	clampCursorToLine();

	if (c == '\n') {
		std::string new_line = lines[cursor.row].substr(cursor.col);
		lines[cursor.row].erase(cursor.col);
		lines.insert(lines.begin() + cursor.row + 1, new_line);
		++cursor.row;
		cursor.col = 0;
	} else if (c == '\t') {
		int spaces = tabSize - (cursor.col % tabSize);
		for (int i = 0; i < spaces; ++i) {
			insertChar(' ');
		}
	} else {
		lines[cursor.row].insert(lines[cursor.row].begin() + cursor.col, c);
		++cursor.col;
	}
}

void TextArea::clampCursorToLine() {
	cursor = getCharPosClamped(cursor);
}

CharPos TextArea::getCharPosClamped(CharPos pos) const {
	pos.col = std::min(pos.col, lines[pos.row].length()); // TODO: handle invalid pos
	return pos;
}

void TextArea::updateSelection(bool select) {
	if (select) {
		if (!isSelectionMode) {
			isSelectionMode = true;
			selectionStartCharPos = getCharPosClamped(cursor);
		}
	} else {
		isSelectionMode = false;
	}
}

void TextArea::deleteSelection() {
	clampCursorToLine();

	CharPos start = std::min(cursor, selectionStartCharPos);
	CharPos end = std::max(cursor, selectionStartCharPos);

	if (start.row == end.row) {
		lines[start.row].erase(start.col, end.col - start.col);		
	} else {
		lines[start.row].erase(start.col);
		lines[start.row] += lines[end.row].substr(end.col);
		lines[end.row].erase(0, end.col);
		lines.erase(lines.begin() + start.row + 1, lines.begin() + end.row + 1);
	}

	cursor = start;
	isSelectionMode = false;
}

/***********************************************************************
    created:    2nd July 2015
    author:     Lukas Meindl

    purpose:    Implementation of EditboxBase class widget
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2015 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/widgets/EditboxBase.h"
#include "CEGUI/falagard/XMLEnumHelper.h" // for DefaultParagraphDirection property helper
#include "CEGUI/text/TextUtils.h"
#include "CEGUI/text/Font.h"
#include "CEGUI/Clipboard.h"
#include "CEGUI/UndoHandler.h"
#if defined(CEGUI_HAS_PCRE_REGEX)
#   include "CEGUI/PCRERegexMatcher.h"
#elif defined(CEGUI_HAS_STD11_REGEX)
#   include "CEGUI/StdRegexMatcher.h"
#endif
#include <string.h>

namespace CEGUI
{
const String EditboxBase::EventNamespace("EditboxBase");
const String EditboxBase::WidgetTypeName("CEGUI/EditboxBase");
const String EditboxBase::EventReadOnlyModeChanged("ReadOnlyModeChanged");
const String EditboxBase::EventTextMaskingEnabledChanged("TextMaskingEnabledChanged");
const String EditboxBase::EventTextMaskingCodepointChanged("TextMaskingCodepointChanged");
const String EditboxBase::EventMaximumTextLengthChanged("MaximumTextLengthChanged");
const String EditboxBase::EventDefaultParagraphDirectionChanged("DefaultParagraphDirectionChanged");
const String EditboxBase::EventCaretMoved("CaretMoved");
const String EditboxBase::EventTextSelectionChanged("TextSelectionChanged");
const String EditboxBase::EventEditboxFull("EditboxFull");
const String EditboxBase::EventValidationStringChanged("ValidationStringChanged");
const String EditboxBase::EventTextValidityChanged("TextValidityChanged");

//----------------------------------------------------------------------------//
static RegexMatcher* createRegexMatcher()
{
#if defined(CEGUI_HAS_PCRE_REGEX)
    return new PCRERegexMatcher();
#elif defined(CEGUI_HAS_STD11_REGEX)
    return new StdRegexMatcher();
#else
    return nullptr;
#endif
}

//----------------------------------------------------------------------------//
EditboxBase::EditboxBase(const String& type, const String& name)
    : Window(type, name)
    , d_maxTextLen(String().max_size())
    , d_undoHandler(new UndoHandler(this))
{
    d_renderedText.setHorizontalFormatting(d_textFormatting);
    addEditboxBaseProperties();
}

//----------------------------------------------------------------------------//
EditboxBase::~EditboxBase()
{
    if (d_weOwnValidator && d_validator)
        delete d_validator;
}

//----------------------------------------------------------------------------//
void EditboxBase::setReadOnly(bool setting)
{
    if (d_readOnly == setting)
        return;

    d_readOnly = setting;
    WindowEventArgs args(this);
    onReadOnlyChanged(args);
        
    // Update the cursor according to the read only state.
    if (setting)
        setCursor(d_readOnlyCursorImage);
    else
        setCursor(getProperty<Image*>(Window::CursorImagePropertyName));
}

//----------------------------------------------------------------------------//
void EditboxBase::setEnabled(bool enabled)
{
    Window::setEnabled(enabled);

    // Update the mouse cursor according to the read only state.
    if (enabled)
        setCursor(getProperty<Image*>(Window::CursorImagePropertyName));
    else
        setCursor(d_readOnlyCursorImage);
}

//----------------------------------------------------------------------------//
void EditboxBase::setTextMaskingEnabled(bool setting)
{
    if (d_textMaskingEnabled == setting)
        return;

    d_textMaskingEnabled = setting;
    WindowEventArgs args(this);
    onTextMaskingEnabledChanged(args);
}

//----------------------------------------------------------------------------//
void EditboxBase::setTextMaskingCodepoint(std::uint32_t code_point)
{
    if (code_point == d_textMaskingCodepoint)
        return;

    d_textMaskingCodepoint = code_point;
    WindowEventArgs args(this);
    onTextMaskingCodepointChanged(args);
}

//----------------------------------------------------------------------------//
void EditboxBase::setMaxTextLength(size_t maxLen)
{
    if (d_maxTextLen == maxLen)
        return;

    d_maxTextLen = maxLen;

    // Trigger max length changed event
    WindowEventArgs args(this);
    onMaximumTextLengthChanged(args);

    // Trim string
    const auto& text = getText();
    if (text.size() > d_maxTextLen)
    {
        setText(text.substr(0, d_maxTextLen));
        d_undoHandler->clearUndoHistory();
        handleValidityChangeForString(getText());
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::setTextFormatting(HorizontalTextFormatting format)
{
    if (d_textFormatting == format)
        return;

    bool wordWrap = false;
    d_textFormatting = decomposeHorizontalFormatting(format, wordWrap);

    d_renderedText.setHorizontalFormatting(d_textFormatting);
    invalidate();
}

//----------------------------------------------------------------------------//
void EditboxBase::setDefaultParagraphDirection(DefaultParagraphDirection defaultParagraphDirection)
{
    if (defaultParagraphDirection == d_defaultParagraphDirection)
        return;

    d_defaultParagraphDirection = defaultParagraphDirection;

    notifyScreenAreaChanged();

    WindowEventArgs eventArgs(this);
    fireEvent(EventDefaultParagraphDirectionChanged, eventArgs, EventNamespace);
}

//----------------------------------------------------------------------------//
RegexMatchState EditboxBase::getStringMatchState(const String& str) const
{
    return d_validator ? d_validator->getMatchStateOfString(str) : RegexMatchState::Valid;
}

//----------------------------------------------------------------------------//
void EditboxBase::setValidator(RegexMatcher* validator)
{
    if (d_validator == validator)
        return;

    if (d_weOwnValidator && d_validator)
        delete d_validator;

    d_validator = validator;

    if (d_validator)
        d_weOwnValidator = false;
    else
    {
        d_validator = createRegexMatcher();
        if (d_validator && !d_validationString.empty())
            d_validator->setRegexString(d_validationString);
        d_weOwnValidator = true;
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::setValidationString(const String& validationString)
{
    if (validationString == d_validationString)
        return;

    if (!d_validator)
        d_validator = createRegexMatcher();

    if (d_validator)
        d_validator->setRegexString(validationString);

    d_validationString = validationString;

    WindowEventArgs args(this);
    onValidationStringChanged(args);

    handleValidityChangeForString(getText());
}

//----------------------------------------------------------------------------//
void EditboxBase::setCaretIndex(size_t caretPos)
{
    // Make sure new position is valid
    const auto textLen = getText().length();
    if (caretPos > textLen)
        caretPos = textLen;

    if (d_caretPos == caretPos)
        return;

    d_caretPos = caretPos;

    WindowEventArgs args(this);
    onCaretMoved(args);
}

//----------------------------------------------------------------------------//
void EditboxBase::setSelection(size_t start_pos, size_t end_pos)
{
    const auto textLen = getText().length();

    // ensure selection start point is within the valid range
    if (start_pos > textLen)
        start_pos = textLen;

    // ensure selection end point is within the valid range
    if (end_pos > textLen)
        end_pos = textLen;

    // ensure start is before end
    if (start_pos > end_pos)
        std::swap(start_pos, end_pos);

    if (start_pos != d_selectionStart || end_pos != d_selectionEnd)
    {
        d_selectionStart = start_pos;
        d_selectionEnd = end_pos;

        WindowEventArgs args(this);
        onTextSelectionChanged(args);
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::setSelectionStart(size_t start_pos)
{
	setSelection(start_pos, start_pos + getSelectionLength());
}

//----------------------------------------------------------------------------//
void EditboxBase::setSelectionLength(size_t length)
{
    const auto start = getSelectionStart();
	setSelection(start, start + length);
}

//----------------------------------------------------------------------------//
void EditboxBase::clearSelection()
{
    if (getSelectionLength())
        setSelection(0, 0);
}

//----------------------------------------------------------------------------//
RenderedText& EditboxBase::getRenderedText() const
{
    if (d_renderedTextDirty)
    {
        if (d_textMaskingEnabled)
        {
            const String maskedText(getText().size(), static_cast<char32_t>(d_textMaskingCodepoint));
            d_renderedText.renderText(maskedText, nullptr, getActualFont(), d_defaultParagraphDirection);
        }
        else
        {
            d_renderedText.renderText(getText(), nullptr, getActualFont(), d_defaultParagraphDirection);
        }

        d_renderedTextDirty = false;
    }

    return d_renderedText;
}

//----------------------------------------------------------------------------//
bool EditboxBase::performCopy(Clipboard& clipboard)
{
    if (!getSelectionLength())
        return false;

    clipboard.setText(getText().substr(getSelectionStart(), getSelectionLength()));
    return true;
}

//----------------------------------------------------------------------------//
bool EditboxBase::performCut(Clipboard& clipboard)
{
    if (isReadOnly())
        return false;

    if (!performCopy(clipboard))
        return false;

    handleDelete();
    return true;
}

//----------------------------------------------------------------------------//
bool EditboxBase::performPaste(Clipboard& clipboard)
{
    if (isReadOnly())
        return false;

    const String clipboardText = clipboard.getText();
    if (clipboardText.empty())
        return false;

    String tmp = getText();
    if (tmp.size() + clipboardText.size() - getSelectionLength() > d_maxTextLen)
    {
        WindowEventArgs args(this);
        onEditboxFullEvent(args);
        return false;
    }

    UndoHandler::UndoAction undoDeleteSelection;
    const bool hasSelection = !!getSelectionLength();
    if (hasSelection)
    {
        undoDeleteSelection.d_type = UndoHandler::UndoActionType::Delete;
        undoDeleteSelection.d_startIdx = getSelectionStart();
        undoDeleteSelection.d_text = tmp.substr(getSelectionStart(), getSelectionLength());

        tmp.erase(getSelectionStart(), getSelectionLength());
    }

    tmp.insert(getSelectionStart(), clipboardText);

    if (!handleValidityChangeForString(tmp))
        return false;

    UndoHandler::UndoAction undoPaste;
    undoPaste.d_type = UndoHandler::UndoActionType::Insert;
    undoPaste.d_startIdx = getCaretIndex();
    undoPaste.d_text = clipboardText;

    setCaretIndex(d_selectionStart);
    clearSelection();

    // advance caret (done first so we can "do stuff" in event handlers!)
    d_caretPos += clipboardText.size();

    // set text to the newly modified string
    setText(tmp);

    d_undoHandler->addUndoHistory(undoPaste);
    if (hasSelection)
        d_undoHandler->addUndoHistory(undoDeleteSelection);

    return true;
}

//----------------------------------------------------------------------------//
void EditboxBase::onCursorPressHold(CursorInputEventArgs& e)
{
    Window::onCursorPressHold(e);

    if (e.source == CursorInputSource::Left)
    {
        if (captureInput())
        {
            clearSelection();
            d_dragging = true;
            d_dragAnchorIdx = getTextIndexFromPosition(e.position);
#if defined(CEGUI_BIDI_SUPPORT) && !defined(CEGUI_USE_RAQM)
            //if (getV2lMapping().size() > d_dragAnchorIdx)
            //    d_dragAnchorIdx =
            //        getV2lMapping()[d_dragAnchorIdx];
#endif
            setCaretIndex(d_dragAnchorIdx);
        }

        ++e.handled;
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::onCursorActivate(CursorInputEventArgs& e)
{
    Window::onCursorActivate(e);

    if (e.source == CursorInputSource::Left)
    {
        releaseInput();
        ++e.handled;
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::onCursorMove(CursorInputEventArgs& e)
{
    Window::onCursorMove(e);

    if (d_dragging)
    {
        size_t anchorIdx = getTextIndexFromPosition(e.position);
#if defined(CEGUI_BIDI_SUPPORT) && !defined(CEGUI_USE_RAQM)
        //if (getV2lMapping().size() > anchorIdx)
        //    anchorIdx = getV2lMapping()[anchorIdx];
#endif
        setCaretIndex(anchorIdx);
        setSelection(d_caretPos, d_dragAnchorIdx);
    }

    ++e.handled;
}

//----------------------------------------------------------------------------//
void EditboxBase::onCaptureLost(WindowEventArgs& e)
{
    d_dragging = false;
    Window::onCaptureLost(e);
    ++e.handled;
}

//----------------------------------------------------------------------------//
void EditboxBase::onFontChanged(WindowEventArgs& e)
{
    Window::onFontChanged(e);

    if (d_renderedText.getDefaultFont() != getActualFont())
    {
        d_renderedTextDirty = true;
        invalidate();
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::onTextChanged(WindowEventArgs& e)
{
    Window::onTextChanged(e);

    clearSelection();

    d_renderedTextDirty = true;
    invalidate();

    // Make sure the caret is within the text
    const auto textLen = getText().size();
    if (d_caretPos > textLen)
        setCaretIndex(textLen);

    FIXME; // text is not updated here, but we already want to make caret visible and to update scrollbars!
    //???renderText right now?
    performChildLayout(false, false);
    ensureCaretIsVisible();

    ++e.handled;
}

//----------------------------------------------------------------------------//
void EditboxBase::handleCharLeft(bool select)
{
    if (d_caretPos > 0)
    {
#if CEGUI_STRING_CLASS != CEGUI_STRING_CLASS_UTF_8
        size_t previousCodePointPos = d_caretPos - 1;
#else
        const String& currentText = getText();
        String::codepoint_iterator caretIter(currentText.begin() + d_caretPos,
                                             currentText.begin(), currentText.end());
        --caretIter;

        size_t previousCodePointPos = caretIter.getCodeUnitIndexFromStart();
#endif
        setCaretIndex(previousCodePointPos);
    }

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleWordLeft(bool select)
{
    if (d_caretPos > 0)
        setCaretIndex(TextUtils::getWordStartIndex(getText(), getCaretIndex()));

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleCharRight(bool select)
{
    if (d_caretPos < getText().length())
    {
#if CEGUI_STRING_CLASS != CEGUI_STRING_CLASS_UTF_8
        size_t codePointSize = 1;
#else
        const CEGUI::String& currentText = getText();
        size_t codePointSize = String::getCodePointSize(currentText[d_caretPos]);
#endif
        setCaretIndex(d_caretPos + codePointSize);
    }

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleWordRight(bool select)
{
    if (d_caretPos < getText().length())
        setCaretIndex(TextUtils::getNextWordStartIndex(getText(), d_caretPos));

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleHome(bool select)
{
    if (d_caretPos > 0)
        setCaretIndex(0);

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleEnd(bool select)
{
    const auto textLen = getText().length();
    if (d_caretPos < textLen)
        setCaretIndex(textLen);

    if (select)
        setSelection(d_caretPos, d_dragAnchorIdx);
    else
        clearSelection();
}

//----------------------------------------------------------------------------//
void EditboxBase::handleSelectAll()
{
    const auto textLen = getText().length();
    setSelection(0, textLen);
    setCaretIndex(textLen);
}

//----------------------------------------------------------------------------//
void EditboxBase::handleBackspace()
{
    if (isReadOnly())
        return;

    String tmp(getText());

    //!!!FIXME TEXT: lots of duplication between delete & backspace!
    if (getSelectionLength())
    {
        UndoHandler::UndoAction undoDeleteSelection;
        undoDeleteSelection.d_type = UndoHandler::UndoActionType::Delete;
        undoDeleteSelection.d_startIdx = getSelectionStart();
        undoDeleteSelection.d_text = tmp.substr(getSelectionStart(), getSelectionLength());

        tmp.erase(getSelectionStart(), getSelectionLength());

        if (handleValidityChangeForString(tmp))
        {
            setCaretIndex(d_selectionStart);
            clearSelection();

            setText(tmp);
            d_undoHandler->addUndoHistory(undoDeleteSelection);
        }
    }
    else if (getCaretIndex() > 0)
    {
        UndoHandler::UndoAction undo;
        undo.d_type = UndoHandler::UndoActionType::Delete;

#if CEGUI_STRING_CLASS != CEGUI_STRING_CLASS_UTF_8
        size_t deleteStartPos = d_caretPos - 1;
        size_t deleteLength = 1;
#else
        String::codepoint_iterator caretIter(tmp.begin() + d_caretPos,
            tmp.begin(), tmp.end());
        --caretIter;

        size_t deleteStartPos = caretIter.getCodeUnitIndexFromStart();
        size_t deleteLength = d_caretPos - deleteStartPos;
#endif

        undo.d_startIdx = deleteStartPos;
        undo.d_text = tmp.substr(deleteStartPos, deleteLength);

        tmp.erase(deleteStartPos, deleteLength);

        if (handleValidityChangeForString(tmp))
        {
            setCaretIndex(deleteStartPos);

            // set text to the newly modified string
            setText(tmp);
            d_undoHandler->addUndoHistory(undo);
        }
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::handleDelete()
{
    if (isReadOnly())
        return;

    String tmp(getText());

    if (getSelectionLength())
    {
        UndoHandler::UndoAction undoSelection;
        undoSelection.d_type = UndoHandler::UndoActionType::Delete;
        undoSelection.d_startIdx = getSelectionStart();
        undoSelection.d_text = tmp.substr(getSelectionStart(), getSelectionLength());

        tmp.erase(getSelectionStart(), getSelectionLength());

        if (handleValidityChangeForString(tmp))
        {
            setCaretIndex(d_selectionStart);
            clearSelection();

            setText(tmp);
            d_undoHandler->addUndoHistory(undoSelection);
        }
    }
    else if (getCaretIndex() < tmp.length())
    {
        UndoHandler::UndoAction undo;
        undo.d_type = UndoHandler::UndoActionType::Delete;
        undo.d_startIdx = d_caretPos;

#if CEGUI_STRING_CLASS != CEGUI_STRING_CLASS_UTF_8
        size_t eraseLength = 1;
#else
        size_t eraseLength = String::getCodePointSize(tmp[d_caretPos]);
#endif

        undo.d_text = tmp.substr(d_caretPos, eraseLength);

        tmp.erase(d_caretPos, eraseLength);

        if (handleValidityChangeForString(tmp))
        {
            setText(tmp);
            d_undoHandler->addUndoHistory(undo);
        }
    }
}

//----------------------------------------------------------------------------//
void EditboxBase::onReadOnlyChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventReadOnlyModeChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onTextMaskingEnabledChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventTextMaskingEnabledChanged , e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onTextMaskingCodepointChanged(WindowEventArgs& e)
{
    if (isTextMaskingEnabled())
        invalidate();

    fireEvent(EventTextMaskingCodepointChanged , e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onMaximumTextLengthChanged(WindowEventArgs& e)
{
    fireEvent(EventMaximumTextLengthChanged , e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onCaretMoved(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventCaretMoved, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onTextSelectionChanged(WindowEventArgs& e)
{
    invalidate();
    fireEvent(EventTextSelectionChanged , e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onEditboxFullEvent(WindowEventArgs& e)
{
    fireEvent(EventEditboxFull, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onValidationStringChanged(WindowEventArgs& e)
{
    fireEvent(EventValidationStringChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
void EditboxBase::onTextValidityChanged(RegexMatchStateEventArgs& e)
{
    fireEvent(EventTextValidityChanged, e, EventNamespace);
}

//----------------------------------------------------------------------------//
size_t EditboxBase::getCaretIndex() const
{
#if defined(CEGUI_BIDI_SUPPORT) && !defined(CEGUI_USE_RAQM)
    //size_t caretPos = d_caretPos;
    //if (getL2vMapping().size() > caretPos)
    //    caretPos = getL2vMapping()[caretPos];
#endif

    return d_caretPos;
}

//----------------------------------------------------------------------------//
bool EditboxBase::performUndo()
{
    bool result = false;
    if (!isReadOnly())
    {
        clearSelection();
        result = d_undoHandler->undo(d_caretPos);
    }

    return result;
}

//----------------------------------------------------------------------------//
bool EditboxBase::performRedo()
{
    bool result = false;
    if (!isReadOnly())
    {
        clearSelection();
        result = d_undoHandler->redo(d_caretPos);
    }

    return result;
}

//----------------------------------------------------------------------------//
bool EditboxBase::handleBasicSemanticValue(SemanticEventArgs& e)
{
    switch (e.d_semanticValue)
    {
    case SemanticValue::DeletePreviousCharacter:
        handleBackspace();
        break;

    case SemanticValue::DeleteNextCharacter:
        handleDelete();
        break;

    case SemanticValue::GoToPreviousCharacter:
        handleCharLeft(false);
        break;

    case SemanticValue::GoToNextCharacter:
        handleCharRight(false);
        break;

    case SemanticValue::SelectPreviousCharacter:
        handleCharLeft(true);
        break;

    case SemanticValue::SelectNextCharacter:
        handleCharRight(true);
        break;

    case SemanticValue::GoToPreviousWord:
        handleWordLeft(false);
        break;

    case SemanticValue::GoToNextWord:
        handleWordRight(false);
        break;

    case SemanticValue::SelectPreviousWord:
        handleWordLeft(true);
        break;

    case SemanticValue::SelectNextWord:
        handleWordRight(true);
        break;

    case SemanticValue::GoToStartOfDocument:
        handleHome(false);
        break;

    case SemanticValue::GoToEndOfDocument:
        handleEnd(false);
        break;

    case SemanticValue::SelectToStartOfDocument:
        handleHome(true);
        break;

    case SemanticValue::SelectToEndOfDocument:
        handleEnd(true);
        break;

    case SemanticValue::SelectAll:
        handleSelectAll();
        break;

    default:
        return false;
    }

    return true;
}

//----------------------------------------------------------------------------//
bool EditboxBase::handleFontRenderSizeChange(const Font& font)
{
    const bool res = Window::handleFontRenderSizeChange(font);

    if (getActualFont() == &font)
    {
        d_renderedTextDirty = true;
        invalidate();
        return true;
    }

    return res;
}

//----------------------------------------------------------------------------//
bool EditboxBase::handleValidityChangeForString(const String& str)
{
    const RegexMatchState newState = getStringMatchState(str);

    if (newState == d_validatorMatchState)
        return d_previousValidityChangeResponse;

    RegexMatchStateEventArgs args(this, newState);
    onTextValidityChanged(args);

    d_previousValidityChangeResponse = !!args.handled;
    d_validatorMatchState = newState;

    return d_previousValidityChangeResponse;
}

//----------------------------------------------------------------------------//
void EditboxBase::addEditboxBaseProperties()
{
    const String& propertyOrigin = WidgetTypeName;

    CEGUI_DEFINE_PROPERTY(EditboxBase, bool,
        "ReadOnly", "Property to get/set the read-only setting for the Editbox.  Value is either \"true\" or \"false\".",
        &EditboxBase::setReadOnly, &EditboxBase::isReadOnly, false
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, bool,
        "TextMaskingEnabled", "Property to get/set the mask text setting for the Editbox.  Value is either \"true\" or \"false\".",
        &EditboxBase::setTextMaskingEnabled, &EditboxBase::isTextMaskingEnabled, false
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, std::uint32_t,
        "TextMaskingCodepoint", "Property to get/set the UTF-32 codepoint value used for masking text. "
        "Value is 32 bit unsigned integer representing an UTF-32 codepoint.",
        &EditboxBase::setTextMaskingCodepoint, &EditboxBase::getTextMaskingCodepoint,
        42
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, size_t,
        "CaretIndex", "Property to get/set the current caret index.  Value is \"[uint]\".",
        &EditboxBase::setCaretIndex, &EditboxBase::getCaretIndex, 0
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, size_t,
        "SelectionStart", "Property to get/set the zero based index of the selection start position within the text.  Value is \"[uint]\".",
        &EditboxBase::setSelectionStart, &EditboxBase::getSelectionStart, 0
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, size_t,
        "SelectionLength", "Property to get/set the length of the selection (as a count of the number of code points selected).  Value is \"[uint]\".",
        &EditboxBase::setSelectionLength, &EditboxBase::getSelectionLength, 0
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, size_t,
        "MaxTextLength", "Property to get/set the the maximum allowed text length (as a count of code points).  Value is \"[uint]\".",
        &EditboxBase::setMaxTextLength, &EditboxBase::getMaxTextLength, String().max_size()
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, Image*,
        "ReadOnlyCursorImage", "Property to get/set the mouse cursor image "
        "for the EditBox when in Read-only mode.  Value should be \"imageset/image_name\". "
        "Value is the image to use.",
        &EditboxBase::setReadOnlyCursorImage, &EditboxBase::getReadOnlyCursorImage, nullptr
    );
    CEGUI_DEFINE_PROPERTY(EditboxBase, HorizontalTextFormatting,
        "TextFormatting", "Property to get/set the horizontal formatting mode. "
        "Value is one of HorizontalTextFormatting enum",
        &EditboxBase::setTextFormatting, &EditboxBase::getTextFormatting,
        HorizontalTextFormatting::LeftAligned);
    CEGUI_DEFINE_PROPERTY(EditboxBase, DefaultParagraphDirection,
        "DefaultParagraphDirection", "Property to get/set the default paragraph direction. "
        "This is only in effect if raqm is linked and activated. It sets the default order of the "
        "words in a paragraph, which is relevant when having sentences in a RightToLeft language that "
        "may start with a word (or to be specific: first character of a word) from a LeftToRight language. "
        "Example: If the mode is set to Automatic and the first word of a paragraph in Hebrew is a German "
        "company name, written in German alphabet, the German word will end up left, whereas the rest of "
        "the Hebrew sentences starts from the righ, continuing towards the left. With the setting RightToLeft "
        "the sentence will start on the very right with the German word, as would be expected in a mainly "
        "RightToLeft written paragraph. If the language of the UI user is known, then either LeftToRight "
        "or RightToLeft should be chosen for the paragraphs. Default is LeftToRight."
        "Value is one of \"LeftToRight\", \"RightToLeft\" or \"Automatic\".",
        &EditboxBase::setDefaultParagraphDirection, &EditboxBase::getDefaultParagraphDirection,
        DefaultParagraphDirection::LeftToRight
    );
}

//----------------------------------------------------------------------------//
bool isSelectionSemanticValue(SemanticValue value)
{
    return (value >= SemanticValue::SelectRange && value <= SemanticValue::SelectToEndOfLine) ||
        (value >= SemanticValue::SelectToStartOfDocument && value <= SemanticValue::SelectToPreviousPage);
}

}

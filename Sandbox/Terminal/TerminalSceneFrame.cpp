#include "stdafx.h"
#include "TerminalSceneFrame.h"
#include "KeyboardEvent.h"
#include "CharEvent.h"
#include "SimpleRenderPipeline.h"
#include "RenderStage.h"
#include "RenderTarget.h"
#include "FontRenderer.h"
#include "ViewPort.h"
#include "StringUtils.h"

typedef GlEngine::Events::KeyboardEvent KeyboardEvent;
typedef GlEngine::Events::KeyboardEventType KeyboardEventType;
typedef GlEngine::Events::CharEvent CharEvent;

const std::string STR_PROMPT = "> "s;
const std::string STR_CURSOR = "|"s;

TerminalSceneFrame::TerminalSceneFrame(Frame *wrapFrame)
    : wrappedFrame(wrapFrame), showTerminal(false), pauseWhenVisible(true), lines(new std::vector<std::string>()), currentLine(""s), cursorPos(0), renderer(nullptr), cursorBlinkDelta(0.f)
{
    lines->push_back("Hello, World!"s);
    lines->push_back("I like chocolate milk!"s);
}
TerminalSceneFrame::~TerminalSceneFrame()
{
}

bool TerminalSceneFrame::Initialize()
{
    if (!Frame::Initialize()) return false;

    auto pipeline = new GlEngine::SimpleRenderPipeline(this, { { GlEngine::renderStage_opaque, nullptr } });
    SetMainPipeline(pipeline);

    return true;
}

void TerminalSceneFrame::Tick(float delta)
{
    if (!showTerminal || !pauseWhenVisible) wrappedFrame->Tick(delta);
    Frame::Tick(delta);
}

void TerminalSceneFrame::HandleEvent(GlEngine::Events::Event &evt)
{
    Frame::HandleEvent(evt);
    if (evt.IsHandled()) return;
    auto charevt = dynamic_cast<CharEvent*>(&evt);
    if (charevt != nullptr)
    {
        if (charevt->chr() == '`')
        {
            showTerminal = !showTerminal;
            charevt->Handle();
            return;
        }
    }

    if (showTerminal)
    {
        auto old = currentLine;
        auto oldCursor = cursorPos;
        auto kbdevt = dynamic_cast<KeyboardEvent*>(&evt);
        if (kbdevt != nullptr && kbdevt->type() == KeyboardEventType::KeyTyped)
        {
            auto vk_code = kbdevt->GetVirtualKeyCode();
            switch (vk_code)
            {
            case VK_UP:
            case VK_DOWN:
                //Ignore, for now
                break;

            case VK_RETURN:
                lines->push_back(currentLine);
                currentLine = "";
                cursorPos = 0;
                break;
            case VK_DELETE:
                if (cursorPos < currentLine.length() && cursorPos == selectionStartPos) selectionStartPos = cursorPos + 1;
                deleteSelection();
                break;

            case VK_HOME:
                cursorPos = 0;
                break;
            case VK_END:
                cursorPos = currentLine.length();
                break;
            case VK_RIGHT:
                if (kbdevt->isControlPressed()) cursorPos = GlEngine::Util::findEndOfWord(currentLine, cursorPos);
                else if (cursorPos < currentLine.length()) cursorPos++;
                break;
            case VK_LEFT:
                if (kbdevt->isControlPressed()) cursorPos = GlEngine::Util::findBeginningOfWord(currentLine, cursorPos);
                else if (cursorPos > 0) cursorPos--;
                break;

            case VK_ALPHANUMERIC<'X'>():
                if (selectionStartPos == cursorPos)
                {
                    setClipboardContents(currentLine);
                    currentLine = ""s;
                    selectionStartPos = cursorPos = 0;
                }
                else
                {
                    setClipboardContents(getSelection());
                    deleteSelection();
                }
                break;
            case VK_ALPHANUMERIC<'C'>():
                if (selectionStartPos == cursorPos) setClipboardContents(currentLine);
                else setClipboardContents(getSelection());
                break;
            case VK_ALPHANUMERIC<'V'>():
                if (kbdevt->isControlPressed()) deleteSelection(getClipboardContents());
                break;
            }

            if (oldCursor != cursorPos && !kbdevt->isShiftPressed()) selectionStartPos = cursorPos;
        }
        else if (charevt != nullptr)
        {
            auto chr = charevt->chr();
            switch (chr)
            {
            case '\x08': //Backspace
                if (cursorPos > 0 && cursorPos == selectionStartPos) selectionStartPos = cursorPos - 1;
                deleteSelection();
                break;

            case '\t': //Tab
                //Ignore, for now
                break;

            default:
                deleteSelection(std::string(1, chr));
            }
        }
        evt.Handle();
        if (oldCursor != cursorPos || old != currentLine) cursorBlinkDelta = 0;
    }
    else
    {
        wrappedFrame->HandleEvent(evt);
    }
}

void TerminalSceneFrame::TickGraphics(float delta)
{
    wrappedFrame->TickGraphics(delta);
    Frame::TickGraphics(delta);
    cursorBlinkDelta = fmod(cursorBlinkDelta + delta, 1.f);
}
void TerminalSceneFrame::UpdateGraphics()
{
    wrappedFrame->UpdateGraphics();
    Frame::UpdateGraphics();
    if (renderer != nullptr) return;
    renderer = new FIG::FontRenderer(new FIG::Font("/Windows/Fonts/consola.ttf"), FIG::FontRendererSettings(12));
}
void TerminalSceneFrame::Render(GlEngine::RenderStage *stage)
{
    if (stage == GlEngine::renderStage_opaque) this->currentRenderTarget->Render(wrappedFrame);
    if (showTerminal && renderer != nullptr)
    {
        Frame::Render(stage);
        auto lineHeight = renderer->lineHeight();
        auto bottom = this->currentRenderTarget->viewPort(stage)->height() - lineHeight;
        auto left = 8;

        Vector<4> color = { 0, 0, 0, 1 };
        Vector<4> bgColor = { 0, 0, 0, 0 };
        Vector<4> selectionBgColor = { .4f, .4f, 1.f, .8f };
        auto renderText = STR_PROMPT + currentLine;
        if (selectionStartPos != cursorPos)
        {
            auto p1 = renderText.substr(0, min(cursorPos, selectionStartPos) + STR_PROMPT.length());
            renderer->DrawDirect(left, (int)floor(bottom), color.getAddr(), bgColor.getAddr(), p1.c_str());
            auto nextLeft = left + renderer->Bounds(p1.c_str()).width();

            auto p2 = getSelection();
            renderer->DrawDirect((int)nextLeft, (int)floor(bottom), color.getAddr(), selectionBgColor.getAddr(), p2.c_str());
            nextLeft += renderer->Bounds(p2.c_str()).width();

            auto p3 = renderText.substr(max(cursorPos, selectionStartPos) + STR_PROMPT.length());
            renderer->DrawDirect((int)nextLeft, (int)floor(bottom), color.getAddr(), bgColor.getAddr(), p3.c_str());
        }
        else renderer->DrawDirect(left, (int)floor(bottom), color.getAddr(), bgColor.getAddr(), renderText.c_str());
        if (cursorBlinkDelta < .5f)
        {
            auto beforeCursorWidth = renderer->Bounds(renderText.substr(0, cursorPos + STR_PROMPT.size()).c_str()).width();
            beforeCursorWidth -= renderer->Bounds(STR_CURSOR.c_str()).width() / 2.f;
            renderer->DrawDirect((int)(left + beforeCursorWidth), (int)floor(bottom), color.getAddr(), STR_CURSOR.c_str());
        }

        bottom -= lineHeight * 1.5f;

        for (int q = lines->size() - 1; q >= 0; q--)
        {
            renderText = lines->at(q);
            renderer->DrawDirect(left, (int)floor(bottom), color.getAddr(), renderText.c_str());
            bottom -= lineHeight;
            if (bottom < 0) break;
        }
    }
}

Vector<3> TerminalSceneFrame::clearColor()
{
    return wrappedFrame->clearColor();
}

void TerminalSceneFrame::deleteSelection(std::string replace)
{
    currentLine = currentLine.substr(0, min(cursorPos, selectionStartPos)) + replace + currentLine.substr(max(cursorPos, selectionStartPos));
    selectionStartPos = cursorPos = min(selectionStartPos, cursorPos) + replace.length();
}
std::string TerminalSceneFrame::getSelection()
{
    return currentLine.substr(min(cursorPos, selectionStartPos), (unsigned)abs((int)cursorPos - (int)selectionStartPos));
}

static std::string clipboardContents = ""s;
std::string TerminalSceneFrame::getClipboardContents()
{
    std::string text;
    if (OpenClipboard(NULL))
    {
        HANDLE clip;
        clip = GetClipboardData(CF_TEXT);
        text = (LPSTR)GlobalLock(clip);
        GlobalUnlock(clip);
        CloseClipboard();
    }
    return text;
}
void TerminalSceneFrame::setClipboardContents(std::string contents)
{
    if (OpenClipboard(NULL))
    {
        EmptyClipboard();
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, contents.size() + 1);
        if (!hg) {
            CloseClipboard();
            return;
        }
        memcpy(GlobalLock(hg), contents.c_str(), contents.size() + 1);
        GlobalUnlock(hg);
        SetClipboardData(CF_TEXT, hg);
        CloseClipboard();
        GlobalFree(hg);
    }
}

/*******************************************************************************************
*
*   rGuiLayout - Layout Code Generator
*
*   MODULE USAGE:
*       #define GUI_WINDOW_CODEGEN_IMPLEMENTATION
*       #include "gui_window_codegen.h"
*
*       INIT: GuiWindowCodegenState state = InitGuiWindowCodegen();
*       DRAW: GuiWindowCodegen(&state);
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2018-2022 raylib technologies (@raylibtech) / Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifndef GUI_WINDOW_CODEGEN_H
#define GUI_WINDOW_CODEGEN_H

typedef struct {

    bool windowActive;
    Rectangle windowBounds;

    bool toolNameEditMode;
    unsigned char toolNameText[64];
    bool toolVersionEditMode;
    unsigned char toolVersionText[64];
    bool companyEditMode;
    unsigned char companyText[64];
    bool toolDescriptionEditMode;
    unsigned char toolDescriptionText[64];

    bool codeTemplateEditMode;
    int codeTemplateActive;

    bool exportAnchorsChecked;
    bool defineRecsChecked;
    bool defineTextsChecked;
    bool fullCommentsChecked;
    bool genButtonFuncsChecked;
    //bool exportStyleChecked;
    //bool embedFontChecked;

    bool btnGenerateCodePressed;
    //bool btnExecuteCodePressed;

    Vector2 codePanelScrollOffset;

    // Custom state variables
    unsigned char *codeText;        // Generated code string
    unsigned int codeHeight;        // Maximum height of code block (computed at drawing)
    Font codeFont;                  // Font used for text drawing

} GuiWindowCodegenState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiWindowCodegenState InitGuiWindowCodegen(void);
void GuiWindowCodegen(GuiWindowCodegenState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_WINDOW_CODEGEN_H

/***********************************************************************************
*
*   GUI_WINDOW_CODEGEN IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_WINDOW_CODEGEN_IMPLEMENTATION)

#include "raygui.h"

#include "font_gohufont.h"

GuiWindowCodegenState InitGuiWindowCodegen(void)
{
    GuiWindowCodegenState state = { 0 };

    state.windowActive = false;
    state.windowBounds = (Rectangle){ 0, 0, 1000, 640 };

    state.toolNameEditMode = false;
    strcpy(state.toolNameText, "layout_name");
    state.toolVersionEditMode = false;
    strcpy(state.toolVersionText, "1.0.0");
    state.companyEditMode = false;
    strcpy(state.companyText, "raylib technologies");
    state.toolDescriptionEditMode = false;
    strcpy(state.toolDescriptionText, "Tool Description");

    state.codeTemplateEditMode = false;
    state.codeTemplateActive = 0;
    state.exportAnchorsChecked = false;
    state.defineRecsChecked = false;
    state.defineTextsChecked = false;
    state.fullCommentsChecked = false;
    state.genButtonFuncsChecked = false;

    state.btnGenerateCodePressed = false;

    state.codePanelScrollOffset = (Vector2){ 0, 0 };

    // Custom variables initialization
    state.codeText = NULL;
    state.codeHeight = 0;
    state.codeFont = LoadFont_Gohufont();       // Font embedded (font_gohufont.h)

    return state;
}

void GuiWindowCodegen(GuiWindowCodegenState *state)
{
    if (state->windowActive)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.8f));

        state->windowBounds = (Rectangle){ GetScreenWidth()/2.0f - 1000/2, GetScreenHeight()/2.0f - 640/2, 1000, 640 };
        state->windowActive = !GuiWindowBox(state->windowBounds, "#7#Code Generation Window");

        GuiGroupBox((Rectangle){ state->windowBounds.x + 765, state->windowBounds.y + 35, 220, 235 }, "Layout Info");
        GuiLabel((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 45, 50, 25 }, "Name:");
        if (GuiTextBox((Rectangle){ state->windowBounds.x + 825, state->windowBounds.y + 45, 150, 25 }, state->toolNameText, 64, state->toolNameEditMode)) state->toolNameEditMode = !state->toolNameEditMode;
        GuiLabel((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 75, 50, 25 }, "Version:");
        if (GuiTextBox((Rectangle){ state->windowBounds.x + 825, state->windowBounds.y + 75, 150, 25 }, state->toolVersionText, 64, state->toolVersionEditMode)) state->toolVersionEditMode = !state->toolVersionEditMode;
        GuiLabel((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 105, 50, 25 }, "Company:");
        if (GuiTextBox((Rectangle){ state->windowBounds.x + 825, state->windowBounds.y + 105, 150, 25 }, state->companyText, 64, state->companyEditMode)) state->companyEditMode = !state->companyEditMode;
        GuiLabel((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 135, 100, 25 }, "Short Description:");
        if (GuiTextBoxMulti((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 160, 200, 100 }, state->toolDescriptionText, 64, state->toolDescriptionEditMode)) state->toolDescriptionEditMode = !state->toolDescriptionEditMode;
        
        GuiGroupBox((Rectangle){ state->windowBounds.x + 765, state->windowBounds.y + 285, 220, 153 }, "Code Generation Options");
        state->exportAnchorsChecked = GuiCheckBox((Rectangle){ state->windowBounds.x + 785, state->windowBounds.y + 335, 15, 15 }, "Export anchors", state->exportAnchorsChecked);
        state->defineRecsChecked = GuiCheckBox((Rectangle){ state->windowBounds.x + 785, state->windowBounds.y + 355, 15, 15 }, "Define Rectangles", state->defineRecsChecked);
        state->defineTextsChecked = GuiCheckBox((Rectangle){ state->windowBounds.x + 785, state->windowBounds.y + 375, 15, 15 }, "Define text as const", state->defineTextsChecked);
        state->fullCommentsChecked = GuiCheckBox((Rectangle){ state->windowBounds.x + 785, state->windowBounds.y + 395, 15, 15 }, "Include detailed comments", state->fullCommentsChecked);
        state->genButtonFuncsChecked = GuiCheckBox((Rectangle){ state->windowBounds.x + 785, state->windowBounds.y + 415, 15, 15 }, "Generate button functions", state->genButtonFuncsChecked);

        state->btnGenerateCodePressed = GuiButton((Rectangle){ state->windowBounds.x + 765, state->windowBounds.y + 455, 220, 30 }, "#7#Export Generated Code");

        // TODO: Add required fields to support a custom code template --> Requires documentation
        //if (GuiDropdownBox((Rectangle){ state->windowBounds.x + 675, state->windowBounds.y + 300, 200, 25 }, "STANDARD CODE FILE (.c);PORTABLE CODE FILE (.h);CUSTOM CODE FILE", &state->codeTemplateActive, state->codeTemplateEditMode)) state->codeTemplateEditMode = !state->codeTemplateEditMode;
        if (GuiDropdownBox((Rectangle){ state->windowBounds.x + 775, state->windowBounds.y + 300, 200, 25 }, "STANDARD CODE FILE (.c);PORTABLE CODE FILE (.h)", &state->codeTemplateActive, state->codeTemplateEditMode)) state->codeTemplateEditMode = !state->codeTemplateEditMode;
        GuiEnable();
        
        // Draw generated code
        if (state->codeText != NULL)
        {
            Rectangle codePanel = { state->windowBounds.x + 10, state->windowBounds.y + 35, 745, 595 };
            Rectangle view = GuiScrollPanel(codePanel, NULL, (Rectangle){ codePanel.x, codePanel.y, codePanel.width*2, (float)state->codeHeight }, &state->codePanelScrollOffset);

            BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
                unsigned int linesCounter = 0;
                unsigned char *currentLine = state->codeText;

                while (currentLine)
                {
                    char *nextLine = strchr(currentLine, '\n');
                    if (nextLine) *nextLine = '\0';     // Temporaly terminating the current line

                    // Only draw lines inside text panel
                    if (((state->codePanelScrollOffset.y + 20*linesCounter) >= 0) &&
                        ((state->codePanelScrollOffset.y + 20*linesCounter) < (codePanel.height - 2)))
                    {
                        DrawTextEx(state->codeFont, currentLine, (Vector2) { codePanel.x + state->codePanelScrollOffset.x + 10, codePanel.y + state->codePanelScrollOffset.y + 20*linesCounter + 8 }, state->codeFont.baseSize, 1, GetColor(GuiGetStyle(TEXTBOX, TEXT_COLOR_NORMAL)));
                    }

                    if (nextLine) *nextLine = '\n';     // Restore newline-char, just to be tidy
                    currentLine = nextLine? (nextLine + 1) : NULL;

                    linesCounter++;
                }
            EndScissorMode();

            state->codeHeight = 20*linesCounter;
        }
    }
}

#endif // GUI_WINDOW_CODEGEN_IMPLEMENTATION

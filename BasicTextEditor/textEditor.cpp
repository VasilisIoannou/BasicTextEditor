#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

/* To DO List */
// Add Up button
// Add Undo


enum STATE{
    NORMAL,
    FILE_HANDLE,
    END
};

STATE programState = FILE_HANDLE;

 std::string newFileName = "";

void printCreateFileText(std::string fileName = ""){
    std::cout << "\033[2J\033[1;1H";
    std::cout<<"Enter the File Name: \n";
    std::cout<<fileName;
}

void inputFileName(KEY_EVENT_RECORD event, std::string& fileName){
    if(event.wVirtualKeyCode == VK_RETURN){
        programState = NORMAL;
        return;
    }
    if(event.wVirtualKeyCode == VK_BACK){
        fileName.pop_back();
        printCreateFileText(fileName);
        return;
    }
    if(event.wVirtualKeyCode == VK_CONTROL){
        programState = END;
        return;
    }
    char c = event.uChar.AsciiChar;
    fileName += c;

    printCreateFileText(fileName);

    return;
}

void closeFunction(){
   std::ofstream file("DATA.txt", std::ios::trunc);
}

void saveDATAFile(std::string fileName){
    std::ofstream DATA("DATA.txt");
    std::cout<<"file opened:"<<fileName<<'\n';
    DATA << fileName;
    DATA.close();
}

std::string readDataFile() {
    std::ifstream data("DATA.txt"); // Open the file for reading
    if (!data.is_open()) {
        std::cerr << "Error: Unable to open the file for reading!\n";
        return "";
    }

    std::string lastFile, line;
    while (std::getline(data, line)) {
        lastFile = line; // Keep overwriting with the current line
    }
    return lastFile; // Return the last line read
}


class controller{
private:
    std::string fileName;
    std::vector<char> text;
    int currentPosition;

    bool controlPressed;

    void setTextColor(std::string color) {
        int colorID;
        if(color == "blue"){
            colorID = 3;
        } else if(color == "red"){
            colorID = 5;
        } else {
        colorID = 7;
        }
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, colorID);
    }

    void addRow(){
        text.push_back('\n');
    }

    void downArrow(){
        int textSize = text.size();
        int startPos = currentPosition;
        if(startPos == 0) startPos++;

        //Find previous /n
        int prevTab = currentPosition;

        if(text[prevTab] == '\n') prevTab--;
        while(text[prevTab] != '\n' && prevTab > 0){
            prevTab--;
        }
        int relativPos  = startPos - prevTab;
        if(prevTab == 0 && currentPosition != 0) relativPos++;

        //Find next /n
        int nextTab = currentPosition;
        if(text[nextTab] == '\n') nextTab++;

        while(text[nextTab] != '\n' && nextTab < textSize -1){
            nextTab ++ ;
        }

        //Find 2nd next /n
        int nextSecondTab = nextTab;
        if(nextSecondTab+1<textSize) nextSecondTab++;

        while(text[nextSecondTab] != '\n' && nextSecondTab < textSize -1){
            nextSecondTab ++ ;
        }

        if(nextTab + relativPos > nextSecondTab && nextTab + relativPos < textSize)
            currentPosition = nextSecondTab;
        else if(nextTab + relativPos < textSize)
            currentPosition = nextTab + relativPos;
        else if(text[currentPosition] == '\n')
            currentPosition--;
    }

    void upArrow(){
        int textSize = text.size();
        int startPos = currentPosition;
        if(startPos == 0) startPos++;

        //Find previous /n
        int prevTab = currentPosition;
        while(text[prevTab] != '\n' && prevTab>1){
            prevTab--;
        }

        int relativPos = startPos - prevTab;

        //Find 2nd previous /n
        int prevSecondTab;
        if(prevTab - 1 > 0)
            prevSecondTab = prevTab - 1;
        else prevSecondTab = 0;

        while(text[prevSecondTab] != '\n' && prevSecondTab > 0){
            prevSecondTab--;
        }

        if(relativPos + prevSecondTab > prevTab && prevTab-1 > 0) currentPosition = prevTab - 1;

        else if(prevSecondTab > 0) currentPosition = prevSecondTab + relativPos;
        else currentPosition = relativPos - 1;

    }

    void saveFile(){
        std::ofstream fSave(fileName);
        if (fSave.is_open()) {
            int textSize = text.size();
            for(int i=0;i<textSize;i++){
                fSave.put(text[i]);
            }
            fSave.close();
        } else {
            std::cerr << "Unable to save file!\n";
        }
    }

    void inputText(KEY_EVENT_RECORD event){
        char inputChar = event.uChar.AsciiChar;
        int NotinsertMode = 1;//If I make Insert Mode in the future = 0

        if(inputChar == '\b'){
            return;
        }

        text.insert(text.begin() + currentPosition + NotinsertMode,inputChar);

        if(inputChar == '('){
            text.insert(text.begin() + currentPosition + 1 + NotinsertMode,')');
        } else if(inputChar == '{'){
            text.insert(text.begin() + currentPosition + 1 + NotinsertMode,'}');
        } else if(inputChar == '['){
            text.insert(text.begin() + currentPosition + 1 + NotinsertMode,']');
        } else if(inputChar == '<'){
            text.insert(text.begin() + currentPosition + 1 + NotinsertMode,'>');
        }

        currentPosition++;
    }

    void controlOptions(KEY_EVENT_RECORD event){
        if(event.uChar.AsciiChar == 'c' || event.uChar.AsciiChar == 'C'){
            programState = END;
        }
        if(event.uChar.AsciiChar == 's' || event.uChar.AsciiChar == 'S'){
            saveFile();
        }
        if(event.uChar.AsciiChar == 'n' || event.uChar.AsciiChar == 'N'){
            newFileName = "";
            programState = FILE_HANDLE;
            printCreateFileText();
        }
        controlPressed = false;
        return;
    }
public:
    controller(std::string setName){
        fileName = setName;
        currentPosition = -1;
        controlPressed = false;
    }

    void setFilename(std::string setName){
        fileName = setName;
    }

    void loadFile(){
        std::ifstream fLoad(fileName, std::ios::in);
        if(fLoad.is_open()){
            char c;
            int tempRow = 0;
            while(fLoad.get(c)){
                text.push_back(c);
                currentPosition++;
            }
            fLoad.close();
        } else {
            std::cerr << "Unable to open file!\n";
            std::cerr << "Does the file exist?\n";
        }

    }

    void handleKeyInput(KEY_EVENT_RECORD event){
        if(controlPressed){
            controlOptions(event);
            return;
        }
        WORD VirtualKey = event.wVirtualKeyCode;
        if(VirtualKey == VK_RETURN){
            if(text.size() == 0) text.push_back('\n');
            text.insert(text.begin()+currentPosition,'\n');
            return;
        }
        if(VirtualKey == VK_BACK){
            if(currentPosition >= 0){
                text.erase(text.begin() + currentPosition);
                currentPosition--;
            }
            return;
        }
        if(VirtualKey == VK_LEFT){
            if(currentPosition >= 0){
                currentPosition--;
            }
            return;
        }
        if(VirtualKey == VK_RIGHT){
            if(currentPosition < text.size() - 1){
                currentPosition++;
            }
            return;
        }
        if(VirtualKey == VK_DOWN){
            downArrow();
            return;
        }
        if(VirtualKey == VK_UP){
            upArrow();
            return;
        }
        if(VirtualKey == VK_CONTROL){
            controlPressed = true;
            return;
        }
        inputText(event);
        return;
    }

    void printText(){
        std::cout << "\033[2J\033[1;1H";
        int currentRow = 1;
        std::cout<<currentRow<<':';
        for(int i=0;i<text.size();i++){
            if(i == currentPosition){
                setTextColor("red");
            }
            if((text[i] == ' ') && i == currentPosition){
                std::cout<<'_';
            } else if(text[i] != '\n') {
                std::cout<<text[i];
            } else if(text[i] == '\n'){
                currentRow++;
                setTextColor("normal");
                std::cout<<'\n'<<currentRow<<':';
                if(i == currentPosition){
                    setTextColor("red");
                    std::cout<<'_';
                }
            }
            setTextColor("normal");
        }
    }

};

int main(){
    // Get the console input handle
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

    if (hStdin == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to get the console input handle.\n";
        return 1;
    }

    DWORD originalMode;
    if (!GetConsoleMode(hStdin, &originalMode)) {
        std::cerr << "Error: Unable to get console mode.\n";
        return 1;
    }

    // Modify the console mode to disable line input and echo input
    DWORD newMode = originalMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    if (!SetConsoleMode(hStdin, newMode)) {
        std::cerr << "Error: Unable to set console mode.\n";
        return 1;
    }

    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(hConsoleOutput, &cursorInfo)) {
        cursorInfo.bVisible = false; // Set caret visibility
        SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
    } else {
        std::cerr << "Error: Unable to retrieve console cursor info.\n";
    }

    INPUT_RECORD inputRecord;
    DWORD eventsRead;

    controller* Control;

    std::string lastFile = readDataFile();
    if(!lastFile.empty()){
        Control = new controller(lastFile);
        Control -> loadFile();
        Control -> printText();
        programState = NORMAL;
    } else {
        printCreateFileText();
        //Make Open File Function
    }


    while (programState != END) {
        if (ReadConsoleInput(hStdin,&inputRecord, 1, &eventsRead)) {
            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {

                KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;

                if (programState == FILE_HANDLE){
                    printCreateFileText();
                    inputFileName(keyEvent,newFileName);

                    if(programState == NORMAL){
                        saveDATAFile(newFileName);
                        Control = new controller(newFileName);
                        Control->loadFile();
                        Control->printText();
                    }
                } else if(programState == NORMAL){
                    Control->handleKeyInput(keyEvent);
                    if(programState == NORMAL){
                        Control->printText();
                    }
                }
            }
        }
    }
    SetConsoleMode(hStdin,originalMode);

    return 0;
}

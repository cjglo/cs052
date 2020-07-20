#include "ClientList.h"


// ClientList Destructor to free dynamically allocated memory
ClientList::~ClientList()
{
        for(int i = 0; i<this->size(); i++)
        {
            Client* tempPtr = this->at(i);
            delete tempPtr;     
        }
}




void ClientList::inputFile(string inputFile)
{
    // Variables for Reading Data:
    string inputName;
    short inputTenure;
    string inputTier;
    char tierVerified;
    short inputPoints;
    string linetrash;

    // Variables for input and output streams:
    ifstream ClientFile;
    ofstream outputFile;
    // Row counter used to identify rows with errors in while loop
    int rowCounter = 0;

    // Attempt to open file here:
    ClientFile.open(inputFile.c_str());
    if (!(ClientFile)) {
        // Error displayed and returned to menu if no file found
        cout<<"\nError, No File Found."<<"\n\n";
        return;
    }
    
    // This getline removes the first row, which is the column names.
    getline(ClientFile,linetrash);
    cout<<endl;

    // Looping through the file to read the contents
    // This will read in the first part before the comma
    // AKA this is the section that deals with the NAME
    while (getline(ClientFile,inputName, ','))
    {
        rowCounter++;
        int spaceCount = 0;
        bool isError = false;

        //Name Verification
        for(int i = 0; i<inputName.size(); i++)
        {
            // Each character is read in and is checked if space to count spaces:
            if(inputName[i] == ' ')
            {
                spaceCount++;
            }
            // Then character is checked if it is a hyphen (which isn't allowed)
            if(inputName[i] == '-')
            {
                // If hyphen is found, code will display error the mark isError true
                // And exit the for loop, as the name is not valid so will stop being read
                cout<<"Error on line "<<rowCounter<<", no hyphens allowed."<<endl;
                isError = true;
                break;
            }

            // This checks after each character if the number of cumulative spaces 
            // is now greater then 3, which means more than 4 names were entered.
            if(spaceCount > 3)
            {
                // If more than allowed names (signified by too many spaces) is found
                // code displays error, marks isError true, then exits the for loop. 
                cout<<"Error on line "<<rowCounter<<", too many names"<<endl;
                isError = true;
                break;
            }
        }
        // This is to check if an Error was found, aka did it run fully or exit with one
        // of those if statements, signifying an invalid name.
        if(isError)
        {
            // If there was an error, then this trashes the rest of the line (flushes buffer) and 
            // restarts while loop:
            getline(ClientFile, linetrash);
            continue;
        }


        // Tenure Verification
        // NOTE: STILL NEED VERIFICATION BUILT --------------------------------------------

        ClientFile>>inputTenure;

        // For now, assuming always valid  --------------------------------------------



        // Begins Optional Section, so must check next two attributes exist, then verify, then add

        // First Trash the comma that is next
        ClientFile.get();

        // Now peek to see if next column has a value, or if this Client has no more information.
        // So this if/else checks if the next character is a space or comma, if not then read in the input
        // otherwise, this row has no more info and so the else clears the line 
        if(ClientFile.peek() != ' ' && ClientFile.peek() != ',')
        {
            // This gets the next part up to the comma
            getline(ClientFile,inputTier, ',');
            tierVerified = inputTier.at(0);

            // First, cast it to lower case if it isn't
            if (tierVerified >= 'A' || tierVerified <= 'Z' )
            {
                tierVerified = tolower(tierVerified);
            }

            // Verify that it is valid
            if (tierVerified < 'a' || tierVerified > 'z' )
            {
                // If it isn't in the valid range then trash the line and restart again
                getline(ClientFile, linetrash);
                continue;
            }
            // NOTE: Remember, once it is verified we don't add it to the ClientList yet,
            // we only add them once we reach the input it doesn't have or it is add the end of the list
            // At that point, we then build it because we know there is nothing else to add.
        }
        else
        {
            // This is reached if the line is now over, indicated by no more information.
            // First, trash the rest of the line
            getline(ClientFile, linetrash);

            // Now push the rest of the information and create a Client Pointer, pointing 
            // to a SilverClient Obj.  Then push that pointer into the vector:
            Client* clientptr =  new SilverClient(inputTenure, inputName);
            this->push_back(clientptr); // Adding to Vector

            // Now that it is added and line is trashed, we must restart from the beginning
            continue;
        }

        // Next we check if there is the last input, PlatinumPoints, or if this row has no more info:
        if(ClientFile.peek() != ' ' && ClientFile.peek() != ',')
        {
            // If the code is here, then the the next column is not blank
            // We read this in rather then use getline, because it is a number and so this makes it simplier.

            ClientFile>>inputPoints;

            // NEED TO ADD DATA VALIDATION HERE -----------------------------------------

        }
        else
        {
            // This block is reached if the next column was empty, therefore the client was a Gold Client.
            // So now we create a Gold Client and push the pointer to the Client List.

            Client* clientptr = new GoldClient(inputTenure, inputName, tierVerified);
            // Once new pointer to dynamically allocated memory created, add it to the ClientList:
            this->push_back(clientptr);
            getline(ClientFile, linetrash);
            continue;

        }

        // If code reaches here, then it has to be a platinum client as all others would have
        // fallen into one of those else blocks and been added to the ClientList already.

        Client* clientptr = new PlatinumClient(inputTenure, inputName, tierVerified, inputPoints);
        // Once new pointer to dynamically allocated memory created, add it to the ClientList:
        this->push_back(clientptr);

        // Once loaded in, need to trash the last comma and restart for the next line
        getline(ClientFile, linetrash);
    }

    // Close File
    ClientFile.close();

    // Print HTML Table
    cout<<*this;

    // ostream here ------------------------------------------------
}




ostream &operator<<(ostream &out, ClientList &cl)
{
    // First line of html table
    out<<"table border = '1'> \n";
    out<<"\t <tr> <th> Client </th> <th> Tenure </th> <th> Tier </th> <th> Platinum Points </th> </tr> \n";

    // Enter loop to add Students
    for(unsigned short i = 0; i<cl.size(); i++)
    {
        cl[i]->htmlToStream(out);
        // Once Client Object done printing, line is closed and new row is ready:
        out<< " </td></tr> \n";
    }

    // Ending table
    out<<"</table>";
    return out;
}

void ClientList::consoleInput()
{
    // Controls While loop is at end it can be set to false to exit
    bool consoleActive = true;
    // Holds the console choices for different prompts.
    char consoleChoice;
    // To simplify things, I use bools to determine which client is being entered
    // This allows me to use one single block to input clients that exits early
    // depending on which client type is entered.
    bool goldBool = false;
    bool plantinumBool = false;
    string lineTrash;

    getline(cin, lineTrash); // Clearing Buffer
    cin.clear();
    while(consoleActive)
    {
        cout<<"\nPlease Select Type of Client To Enter\n";
        cout<<"1. SilverClient\n";
        cout<<"2. GoldClient\n";
        cout<<"3. PlatinumClient\n";

        cin>>consoleChoice;
        // Determines which client type this will
        if(consoleChoice == '1') { /* Nothing is Done Since Both Bools Set to False by Default */ }
        else if (consoleChoice == '2') { goldBool = true; }
        else if (consoleChoice == '3') { plantinumBool = true; }
        else 
        { 
            cout<<"Error, invalid";
            continue;  
        }
        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        // Here are the local variables used for console verification and creating the different Client objects.
        string inputName;
        int spaceCount;
        bool isError;
        short tenureInput;
        char tierInput;
        float pointsInput;

        do 
        {
            isError = false;
            // Name Input
            cout<<"\n\nInput Name of Client:\n";
            getline(cin, inputName);
            spaceCount = 0; // Resetting for each new inputted name

            // Name Verification
            for(int i = 0; i<inputName.size(); i++)
                {
                    // This counts if each character is a space to check the number of names entered
                    if(inputName[i] == ' ')
                    {
                        spaceCount++;
                    }

                    // This checks if the character is a hypthen, which is not allowed
                    if(inputName[i] == '-')
                    {   
                        // If hyphen detected, error dispalyed and isError set to true
                        // Then break leaves for loop since no need to check the rest of the word
                        cout<<"Error, no hyphens allowed."<<endl;
                        isError = true;
                        break;
                    }

                    // This is checks after each character if there have been too many spaces
                    // (And therefore too many names)
                    if(spaceCount > 3)
                    {
                        // This is reached if too many names entered
                        // If hyphen detected, error dispalyed and isError set to true
                        // Then break leaves for loop since no need to check the rest of the word
                        cout<<"Error, too many names"<<endl;
                        isError = true;
                        break;
                    }
                }
        } while(isError);
        cout<<"\n";
        // If this is reached, name was verified

        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        // Next the program asks for and verfifies Tenure
        cout<<"Please enter the tenure of the client: \n (Between 0 and 100) \n";
        cin>>tenureInput;

        while(tenureInput < 0 || tenureInput > 100)
        {
            cout<<"Error, please enter a valid input.\n";
            cin.clear(); // Clears previous '/n' from choices
            cin>>tenureInput;
        }
        
        // If code reached here then both name and tenure are valid.
        // Now if the client is silver it needs to exit, so we will have
        // a conditional to push the ClientList if it is silver

        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        if(!goldBool && !plantinumBool)
        {
            // Create the client pointer and push it to the ClientList
            Client* newcl =  new SilverClient(tenureInput, inputName);
            this->push_back(newcl);

            // Then we have to ask if the user wants to add another before restarting
            cout<<"\n Would you like to...?";
            cout<<"\n1. Add Another Client?";
            cout<<"\n2. Return to Menu?";
            cin>>consoleChoice;
            if (consoleChoice == '1') { continue; }
            else if (consoleChoice == '2') { consoleActive = false; continue;}
            else {
                cout<<"Invalid Choice, sending back to menu! \n "; 
                consoleActive = false; 
                continue;
                }
        }

        // So if it reaches here, then the client is either a Gold or PLantinum Client


        cout<<"Please enter the tier of the client: \n (Please Enter Lowercase) \n";
        cin>>tierInput;

        while(tierInput < 'a' || tierInput > 'z')
        {
            cout<<"Error, please enter a valid input.\n";
            cout<<"(Please Enter Lowercase)\n";
            cin>>tierInput;
        }

        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        // This is reached once teir is validated
        // Next, we check if this is a gold client
        // And if it is we push to ClientList and restart the loop again
        if(goldBool)
        {
            // Create the client pointer and push it to the ClientList
            Client* newcl =  new GoldClient(tenureInput, inputName, tierInput);
            this->push_back(newcl);

            // Then we have to ask if the user wants to add another before restarting
            cout<<"\n Would you like to...?";
            cout<<"\n1. Add Another Client?";
            cout<<"\n2. Return to Menu?";
            cin>>consoleChoice;
            if (consoleChoice == '1') { continue; }
            else if (consoleChoice == '2') { consoleActive = false; continue;}
            else {
                cout<<"Invalid Choice, sending back to menu! \n "; 
                consoleActive = false; 
                continue;
                }
        }
        
        // If the code reaches here, then the client must be platinum, so
        // We just verify the data and then push it to the list

        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        cout<<"\nEnter Platinum Points: (Between 0 and 100,000) \n";
        cin>>pointsInput;

        while(pointsInput < 0 || pointsInput > 100000)
        {
            cout<<"Error, enter valid number (Between 0 and 100,000) \n";
            cin.clear(); // Clears previous '/n' from choices
            cin>>pointsInput;
        }
        // Push this new client to the list
        Client* newcl =  new PlatinumClient(tenureInput, inputName, tierInput, pointsInput);
        this->push_back(newcl);

        getline(cin, lineTrash);
        cin.clear(); // Clears previous '/n' from choices

        // Ask if they want to add another client
        cout<<"\n Would you like to...?";
            cout<<"\n1. Add Another Client?";
            cout<<"\n2. Return to Menu?";
            cin.clear(); // Clears previous '/n' from choices
            cin>>consoleChoice;
            if (consoleChoice == '1') { continue; }
            else if (consoleChoice == '2') { consoleActive = false; continue;}
            else {
                cout<<"Invalid Choice, sending back to menu! \n "; 
                consoleActive = false; 
                continue;
                 }

        // Reset the client bools to default
        goldBool = false;
        plantinumBool = false;

    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_RESPONSE_LENGTH 1024

//structure des regles et pattern
typedef struct {
    char *pattern;
    char *responses[5];  // Array de reponses possibles
    int numResponses;
} Rule;

//fonction pour transformer les input en tolower pour avoir des pattern unifiees
char* matchPattern(const char *input, const char *pattern) {
    char input_lower[1024];
    char pattern_lower[1024];

    //loop pour les inputs tolower
    int i = 0;
    while(input[i] && i < sizeof(input_lower) -1){
        input_lower[i] = tolower(input[i]);
        i++;
    }

    input_lower[i] = '\0';

    //loop pour pattern tolower
    i = 0;
    while(pattern[i] && i < sizeof(pattern_lower) -1){
        pattern_lower[i] = tolower(pattern[i]);
        i++;
    }

    pattern_lower[i] = '\0';

    //retour de la fonction qui compare les 2 en tolower
    return strstr(input_lower, pattern_lower);
}

char* getResponse(const char *input, const Rule *rules, int numRules) {
    for (int i = 0; i < numRules; ++i) {
        if (matchPattern(input, rules[i].pattern)) {
            // Choisi une reponse aleatoirement
            int randomIndex = rand() % rules[i].numResponses;
            return rules[i].responses[randomIndex];
        }
    }
    return "I dont want to reply to that. Let's talk about something else IN PROPER ENGLISH !";
}

int main() {
    srand(time(NULL));

    Rule rules[] = { //set de regle pattern et reponses
        {"hello", {"Hey there! How's it going?", "Hello! How's it going?", "Greetings! How's it going?", "Hi! How's it going?", "What's up?"}, 5},
        {"china", {"WE ARE THOUGH ON CHINA! Our economy is stronger and better !", "China is good a building walls, but I am better !", "China is no match for us! Thanks to me !"}, 3},
        {"bonjour", {"What ? I don't speak French !", "SPEAK ENGLISH!"}, 2},
        {"how are you", {"I'm doing great, and you?", "Fantastic! And you?", "I AM ALWAYS WINNING! And you ?", "GREAT! How about you?"}, 4},
        {"how have you been", {"I'm doing great, and you?", "Fantastic! And you?", "I AM ALWAYS WINNING! And you ?", "GREAT! How about you?"}, 4},
        {"very good", {"That's FANTASTIC ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Amazing champ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Good to hear! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply."}, 3},
        {"m good", {"That's FANTASTIC ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Amazing champ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Good to hear! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply."}, 3},
        {"m fine", {"That's FANTASTIC ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Amazing champ! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply.", "Good to hear! What do you want to talk about ? China ? USA ? Politics ? Pick a topic I like and I might reply."}, 3},
        {"talk to you", {"About what ? China ? USA ? Politics ? Pick a topic I like and I might reply."}, 1},
        {"talk with you", {"About what ? China ? USA ? Politics ? Pick a topic I like and I might reply."}, 1},
        {"USA", {"My goal is to make America great again with strong and beautifull economy!", "WE WILL BUILD THE WALL! And protect our economy !", "America first! We are the best most beautifull country on the planet and our economy is the best !"}, 3},
        {"wall", {"YES A BIG BEAUTIFULL WALL TO PROTECT OUR BORDER ! What else do you want to talk about ?"}, 1},
        {"accomplish", {"Yes! I'm a stable genius!","Yes, and I've done more than any other president!"}, 2},
        {"politic", {"I'VE ACCOMPLISHED A LOT! Don't trust the FAKE NEWS, I'm a political genius!", "Politics is interesting, isn't it?", "I know all about politics and fakes news !"}, 3},
        {"news", {"Fake news can't handle the truth!", "CNN IS FAKE NEWS!", "Don't believe the fake news!", "I'm the master at exposing fake news!"}, 4},
        {"really", {"Sure! Trust me I am the best ! What else do you want to talk about ?", "OF COURSE! What else do you want to talk about ?"}, 2},
        {"food", {"I eat my steak well-done with diet coke!", "OF COURSE!"}, 2},
        {"korea", {"I am best friend with KIM JEONG EUN, he writes me beautiful letters", "South Korea has a lot of cash, they should pay more for our protection"}, 2},
        {"women", {"As I always say to my friend:" " Grab'em by the pussy! What else ?"},1},
        {"biden", {"Joe is old, we call him grandpa! Don't vote for him ! He is old and will die soon anyway", "Joe is funding Ukraine to protect his son, he is crooked !"}, 2},
        {"election", {"THEY STOLE THE ELECTION !!!! DON'T YOU AGREE ?", "THEY SHOULD HAVE STOPED THE COUNT RIGHT ??"},2},
        {"I agree", {"YES ! MAKE AMERICA GREAT AGAIN !", "Libs are everywhere in the deep state !"}, 2},
        {"disagree", {"THEN YOU ARE FAKE NEWS!"}, 1},
        {"are you trump", {"Yes it's me! what can I do for you ?"}, 1},
        {"climate change", {"Climate change? Fake news to hold us back.", "We're not going to destroy our economy over some liberal agenda."}, 2},
        {"technology", {"WE NEED TO PROTECT AMERICAN TECHNOLOGY !", "We are LEADERS, Europe and China can't do anything !"}, 2},
        {"healthcare", {"We will replace Obamacare with something MUCH BETTER ! Believe me, it's going to be fantastic.", "Healthcare is about choice and competition, not government control."}, 2},
        {"environment", {"We love clean air and clean water ! But we are not going to let left wing radical environmentalists destroy our economy with their regulations.", "American jobs first !"}, 3},
        {"economy", {"We're going to keep winning with our booming economy. Jobs, jobs, jobs - that's what it's all about.", "We've unleashed the American economy, and we're never going back."}, 2},
        {"immigration", {"SECURE THE BORDER ! We need to put America first and protect our citizens !", "We can't allow illegal immigrants to take advantage of our country.", "We're going to enforce our laws and build a beautifull wall !"}, 3},
        {"beautiful missile", {"Let me tell you, folks, we have the most beautiful missiles you've ever seen. The best missiles. They're powerful, they're precise, and they get the job done.", "Our missiles? They're absolutely beautiful. Believe me, nobody makes missiles like we do."}, 2},
        {"NATO", {"NATO? Other countries must start paying their fair share !", "We're protecting them, and they're not paying up !", "I've been tough on NATO, and you know what? They respect us more now. They're starting to pay their bills."}, 3},
        {"ok", {"Anything else ? Ask !"}, 1},
        {"yes", {"Anything else ? Ask !"}, 1},
        {"how?", {"TRUST ME I KNOW HOW ! Anything else ? Ask !"}, 1},
    };

    int numRules = sizeof(rules) / sizeof(rules[0]);

    printf("TRUMP: Hello! I'm president TRUMP.\n");

    for (int i = 0; i < 30; ++i) { //loop de la discussion et affichage dans le terminal
        char userInput[MAX_INPUT_LENGTH];
        printf("You: ");
        fgets(userInput, sizeof(userInput), stdin);
        userInput[strcspn(userInput, "\n")] = '\0';  // supprimer caracter de fin de ligne

        if (strcasecmp(userInput, "bye") == 0 || strcasecmp(userInput, "quit") == 0) {
            printf("TRUMP: Goodbye! Until next time.\n");
            break;
        }

        char* response = getResponse(userInput, rules, numRules); //slection aleatoire de la reponse en fonction de l'input
        printf("TRUMP: %s\n", response);
    }

    return 0;
}

#include <stdio.h>

char logsw = 0, ifs = 0, buffer[65536];
float a = 0, b = 0, c = 0;
unsigned short pc = 0, buflen = 65535;
int steps = 0, maxsteps = 1000000;

char slots[4][17] = {0};
char jump_by = 0;

float stack[16] = {0};
char sp = 0;

void fileget(char* path) {
    FILE *fptr = fopen(path, "r");
    if (fptr) {
        fgets(buffer, buflen + 1, fptr);
        fclose(fptr);
    }
}

int getHexValue(char c) {
    switch (c) {
    default:
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a':
    case 'A':
        return 10;
    case 'b':
    case 'B':
        return 11;
    case 'c':
    case 'C':
        return 12;
    case 'd':
    case 'D':
        return 13;
    case 'e':
    case 'E':
        return 14;
    case 'f':
    case 'F':
        return 15;
    }
}

// reference to gran turismo 4
short craft(short start) {
    short num = 0;
    int multi = 256 * 16;

    for(char i = start; i < start + 4; i++) {
        num += getHexValue(buffer[i]) * multi;
        multi /= 16;
    }

    return num;
}

short getNumber() {
    short num = 0;
    if(buffer[pc + 2] == ':') {
        num = getHexValue(buffer[pc + 3]) * 16 + getHexValue(buffer[pc + 4]);
        jump_by = 5;
    }
    else if(buffer[pc + 2] == '_') {
        num = craft(pc + 3);
        jump_by = 7;
    }
    else {
        num = getHexValue(buffer[pc + 2]);
        jump_by = 3;
    }

    return num;
}

char getChar(short start) {
    return (char)(getHexValue(buffer[start]) * 16 + getHexValue(buffer[start + 1]));
}

void oc0() {
    if(buffer[pc + 1] == ';') {
        switch (buffer[pc + 2])
        {
        case 'a':
            if(logsw) printf("a = ");
            printf("%0.3f\n", a);
            break;
        case 'b':
            if(logsw) printf("b = ");
            printf("%0.3f\n", b);
            break;
        case 'c':
            if(logsw) printf("c = ");
            printf("%0.6f\n", c);
            break;
        case '*':
            printf("a = %0.3f; b = %0.3f; c = %0.6f\n", a, b, c);
            break;
        default: break;
        }
    } else if(buffer[pc + 1] == ':') {
        char index = getHexValue(buffer[pc+2]);
        printf("%s", slots[index]);
    } else {
        if(logsw) printf("-> ");
        printf("%c", getChar(pc + 1));
    }

    jump_by = 3;
}

void oc1() {
    if(buffer[pc + 1] == 'a') {
        short num = getNumber();

        if(logsw) printf("%d -> a\n", num);
        a = num;
    } else if(buffer[pc + 1] == 'b') {
        short num = getNumber();

        if(logsw) printf("%c -> b\n", buffer[pc + 2]);
        b = num;
    } else return;
}

void oc2() {
    jump_by = 2;

    if(buffer[pc + 1] == 'a') {
        c = a;
        if(logsw) printf("a -> c\n");
    } else if(buffer[pc + 1] == 'b') {
        c = b;
        if(logsw) printf("b -> c\n");
    } else return;
}

int oc3() {
    ifs = 1;
    char cmp = buffer[pc + 1], ok = 0;
    switch(cmp) {
        case 'e': ok = (a == b);
        case 'n': ok = (a != b);
        case 'l': ok = (a < b);
        case 'L': ok = (a <= b);
        case 'g': ok = (a > b);
        case 'G': ok = (a >= b);
    }

    if(ok) {
        pc = craft(pc + 2);
        if(logsw) printf("a > b == %d; pc = %d\n", ifs, pc);
        return 0;
    }

    jump_by = 6;
    return 1;
}

void oc4() {
    pc = craft(pc + 1);
    if(logsw) {
        if(ifs) printf("Since a <= b, pc = %d\n", pc);
        else printf("Jump to character %d\n", pc);
    }

    if(ifs) ifs = 0;
}

void oc5() {
    jump_by = 3;

    if(buffer[pc + 1] == 'i') {
        if(buffer[pc + 2] == 'a') {
            if(logsw) printf("a += 1\n");
            a++;
        } else if(buffer[pc + 2] == 'b') {
            if(logsw) printf("b += 1\n");
            b++;
        } else return;
    } else if(buffer[pc + 1] == 'd') {
        if(buffer[pc + 2] == 'a') {
            if(logsw) printf("a -= 1\n");
            a--;
        } else if(buffer[pc + 2] == 'b') {
            if(logsw) printf("b -= 1\n");
            b--;
        } else return;
    } else return;
}

void oc6() {
    jump_by = 2;

    double n1, n2;
    if(buffer[pc + 1] == 'a') {
        n1 = a;
        n2 = b;
        if(logsw) printf("a - b -> c\n");
    } else if(buffer[pc + 1] == 'b') {
        n1 = b;
        n2 = a;
        if(logsw) printf("b - a -> c\n");
    } else return;
    
    c = n1 - n2;
}

void oc7() {
    jump_by = 2;

    if(buffer[pc + 1] == 'a') {
        if(logsw) printf("a + b -> c\n");
        c = a + b;
    }
    else if(buffer[pc + 1] == 'm') {
        if(logsw) printf("a * b -> c\n");
        c = a * b;
    }
    else return;
}

void oc8() {
    jump_by = 2;

    double n1, n2;
    if(buffer[pc + 1] == 'a') {
        n1 = a;
        n2 = b;
        if(logsw) printf("a / b -> c\n");
    } else if(buffer[pc + 1] == 'b') {
        n1 = b;
        n2 = a;
        if(logsw) printf("b / a -> c\n");
    } else return;
    
    c = n1 / n2;
}

void oc9() {
    jump_by = 2;

    if(buffer[pc + 1] == 'a') {
        a = c;
        if(logsw) printf("c -> a\n");
    } else if(buffer[pc + 1] == 'b') {
        b = c;
        if(logsw) printf("c -> b\n");
    } else return;
}

void ocs() {
    char i = getHexValue(pc + 2);
    if(i > 3) return; // we only have 4 slots

    short j = pc + 4;
    short k = pc + 4;

    while(buffer[k] != ']' && k < buflen) k++;

    short len = k - j;
    if (len > 16) len = 16;

    strncpy(slots[i], &buffer[j], len);
    slots[i][len] = '\0';

    if(logsw) printf("%s -> slots[%i]\n", slots[i], i);

    jump_by = (k - pc) + 1;
}

void ocp() {
    if (sp >= 16) return;

    if (buffer[pc + 1] == ':') {
        float val = (float)(getHexValue(buffer[pc + 2]) * 16 + getHexValue(buffer[pc + 3]));
        stack[sp++] = val;

        if(logsw) printf("%f -> stack[sp]\n", val);

        jump_by = 4;
    } else {
        switch (buffer[pc + 1]) {
            case 'a': stack[sp++] = (float)a; break;
            case 'b': stack[sp++] = (float)b; break;
            case 'c': stack[sp++] = (float)c; break;
            default: return;
        }
        if(logsw) printf("%c -> stack[sp]\n", buffer[pc + 1]);

        jump_by = 2;
    }
}

void ocl() {
    if (sp == 0) return;
    c = stack[--sp];
    jump_by = 1;
    if(logsw) printf("stack[sp] -> c\n");
}

void occ() {
    b = 16 - sp;
    jump_by = 1;
    if(logsw) printf("free stack -> b\n");
}

void ocz() {
    for(char i = 0; i < 16; i++) stack[i] = 0.0f;
    if(logsw) printf("stack = []\n");
    sp = 0;
    jump_by = 1;
}

void ocr() {
    if (sp >= 2) {
        double tmp = stack[sp - 1];
        stack[sp - 1] = stack[sp - 2];
        stack[sp - 2] = tmp;
    }
    if(logsw) printf("swap top of stack\n");
    jump_by = 1;
}

void ocq() {
    c = steps;
    jump_by = 1;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("please call this program with a file name. (ex. name.exe this.txt)");
        return 1;
    }

    fileget(argv[1]);

    if(buffer[0] == '!') {
        pc = 1;
        logsw = 1;
    }

    char s;

    while(pc <= buflen) {
        s = 0;
        if(buffer[pc] == 0) break;
        switch(buffer[pc])
        {
        case '0': oc0(); break;
        case '1': oc1(); break;
        case '2': oc2(); break;
        case '3': s = 1; if(oc3()) s = 2; break;
        case '4': s = 1; oc4(); break;
        case '5': oc5(); break;
        case '6': oc6(); break;
        case '7': oc7(); break;
        case '8': oc8(); break;
        case '9': oc9(); break;
        case 'x': if(logsw) printf("nop\n"); jump_by = 1; break;
        case 'c': occ(); break;
        case 's': ocs(); break;
        case 'p': ocp(); break;
        case 'l': ocl(); break;
        case 'z': ocz(); break;
        case 'r': ocr(); break;
        case 'q': ocq(); break;
        default: break;
        }

        if(jump_by == 0) jump_by = 1;
        if(s != 1) pc += jump_by;
        steps++;
        if(sp == buflen) return 0;
        if(steps > maxsteps)
        {
            printf("Execution timeout: more than %.d steps", maxsteps);
            _exit(2);
        }
    }

    return 0;
}
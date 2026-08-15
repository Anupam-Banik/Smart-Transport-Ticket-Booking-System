#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#define USER_FILE      "users.txt"

#define MAX_NAME       50
#define MAX_PHONE      20
#define MAX_PASSWORD   33
#define MAX_ROLE       15

/* ====================================================
                   USER STRUCTURE
===================================================== */
typedef struct {
    int id;
    char username[MAX_NAME];
    char password[MAX_PASSWORD];
    char role[MAX_ROLE];
    char phone[MAX_PHONE];
} User;

/* ====================================================
                GLOBAL SESSION VARIABLES
===================================================== */
char currentUser[MAX_NAME] = "";
char currentRole[MAX_ROLE] = "";
int currentUserId = 0;

/* ====================================================
               MD5 HASHING IMPLEMENTATION
===================================================== */
typedef struct {
    uint32_t state[4];
    uint64_t bitCount;
    unsigned char buffer[64];
} MD5_CTX;

uint32_t K[64] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
    0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
    0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
    0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
    0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
    0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
    0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
    0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
    0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
};

int shift[64] = {
    7,12,17,22,7,12,17,22, 7,12,17,22,7,12,17,22,
    5,9,14,20,5,9,14,20,   5,9,14,20,5,9,14,20,
    4,11,16,23,4,11,16,23, 4,11,16,23,4,11,16,23,
    6,10,15,21,6,10,15,21, 6,10,15,21,6,10,15,21
};

uint32_t leftRotate(uint32_t x, uint32_t c) {
    return (x << c) | (x >> (32 - c));
}

void md5Init(MD5_CTX *ctx) {
    ctx->bitCount = 0;
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

void md5Transform(MD5_CTX *ctx, unsigned char data[]) {
    uint32_t a, b, c, d, f, g, temp;
    uint32_t M[16];

    for(int i = 0; i < 16; i++) {
        M[i] = (uint32_t)data[i*4] |
               ((uint32_t)data[i*4+1] << 8) |
               ((uint32_t)data[i*4+2] << 16) |
               ((uint32_t)data[i*4+3] << 24);
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];

    for(int i = 0; i < 64; i++) {
        if(i < 16) {
            f = (b & c) | ((~b) & d);
            g = i;
        } else if(i < 32) {
            f = (d & b) | ((~d) & c);
            g = (5*i + 1) % 16;
        } else if(i < 48) {
            f = b ^ c ^ d;
            g = (3*i + 5) % 16;
        } else {
            f = c ^ (b | (~d));
            g = (7*i) % 16;
        }

        temp = d;
        d = c;
        c = b;
        b = b + leftRotate(a + f + K[i] + M[g], shift[i]);
        a = temp;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
}

void md5Update(MD5_CTX *ctx, unsigned char data[], size_t len) {
    size_t index = (ctx->bitCount / 8) % 64;
    ctx->bitCount += len * 8;
    size_t partLen = 64 - index;
    size_t i = 0;

    if(len >= partLen) {
        memcpy(&ctx->buffer[index], data, partLen);
        md5Transform(ctx, ctx->buffer);

        for(i = partLen; i + 63 < len; i += 64) {
            md5Transform(ctx, &data[i]);
        }
        index = 0;
    }
    memcpy(&ctx->buffer[index], &data[i], len - i);
}

void md5Final(unsigned char hash[], MD5_CTX *ctx) {
    unsigned char padding[64] = {0x80};
    unsigned char bits[8];

    for(int i = 0; i < 8; i++) {
        bits[i] = (ctx->bitCount >> (8*i)) & 0xff;
    }

    size_t index = (ctx->bitCount / 8) % 64;
    size_t padLen = (index < 56) ? (56 - index) : (120 - index);

    md5Update(ctx, padding, padLen);
    md5Update(ctx, bits, 8);

    for(int i = 0; i < 4; i++) {
        hash[i]      = (ctx->state[0] >> (8*i)) & 0xff;
        hash[i+4]    = (ctx->state[1] >> (8*i)) & 0xff;
        hash[i+8]    = (ctx->state[2] >> (8*i)) & 0xff;
        hash[i+12]   = (ctx->state[3] >> (8*i)) & 0xff;
    }
}

void md5Hash(char password[], char output[]) {
    MD5_CTX ctx;
    unsigned char digest[16];

    md5Init(&ctx);
    md5Update(&ctx, (unsigned char *)password, strlen(password));
    md5Final(digest, &ctx);

    for(int i = 0; i < 16; i++) {
        sprintf(&output[i*2], "%02x", digest[i]);
    }
    output[32] = '\0';
}

/* ====================================================
           INITIALIZE DEFAULT ADMIN ACCOUNT
===================================================== */
void initAdmin() {
    FILE *fp;
    User u;
    int found = 0;

    fp = fopen(USER_FILE, "r");
    if(fp != NULL) {
        while(fscanf(fp, "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]\n",
                     &u.id, u.username, u.password, u.role, u.phone) == 5) {
            if(strcmp(u.role, "admin") == 0) {
                found = 1;
                break;
            }
        }
        fclose(fp);
    }

    if(found == 0) {
        char hash[33];
        md5Hash("admin123", hash);

        fp = fopen(USER_FILE, "a");
        if(fp == NULL) {
            printf("User File Error!\n");
            return;
        }
        fprintf(fp, "1|admin|%s|admin|01700000000\n", hash);
        fclose(fp);
    }
}

/* ====================================================
         1 & 3: REGISTER USER (Customer / Owner)
===================================================== */
void registerUser(const char *role) {
    FILE *fp;
    User u, temp;
    char rawPassword[50];
    int maxId = 0;

    fp = fopen(USER_FILE, "a+");
    if(fp == NULL) {
        printf("\nUser File Open Error.\n");
        return;
    }

    rewind(fp);

    printf("\n=====================================\n");
    printf("          %s REGISTRATION\n", role);
    printf("=====================================\n");

    printf("Username : ");
    scanf(" %[^\n]", u.username);

    /* চেক করা হচ্ছে ইউজারনেম আগে থেকেই আছে কি না */
    while(fscanf(fp, "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]\n",
                 &temp.id, temp.username, temp.password, temp.role, temp.phone) == 5) {
        if(temp.id > maxId) {
            maxId = temp.id;
        }

        if(strcasecmp(temp.username, u.username) == 0) {
            printf("\nUsername Already Exists.\n");
            fclose(fp);
            return;
        }
    }

    u.id = maxId + 1;
    strcpy(u.role, role);

    printf("Phone Number : ");
    scanf(" %[^\n]", u.phone);

    printf("Password : ");
    scanf(" %[^\n]", rawPassword);

    /* পাসওয়ার্ড MD5 হ্যাশে কনভার্ট করা হচ্ছে */
    md5Hash(rawPassword, u.password);

    /* ফাইলে ইউজার ডাটা সংরক্ষণ */
    fprintf(fp, "%d|%s|%s|%s|%s\n", u.id, u.username, u.password, u.role, u.phone);
    fclose(fp);

    printf("\n=====================================\n");
    printf("      Registration Successful!\n");
    printf("      User ID  : %d\n", u.id);
    printf("      Username : %s\n", u.username);
    printf("      Role     : %s\n", u.role);
    printf("=====================================\n");
}

/* ====================================================
       2, 4 & 5: LOGIN (Customer / Owner / Admin)
===================================================== */
int login(const char *role) {
    FILE *fp;
    User u;
    char username[MAX_NAME];
    char password[50];
    char hash[33];

    fp = fopen(USER_FILE, "r");
    if(fp == NULL) {
        printf("\nUser File Not Found.\n");
        return 0;
    }

    printf("\n=====================================\n");
    printf("             %s LOGIN\n", role);
    printf("=====================================\n");

    printf("Username : ");
    scanf(" %[^\n]", username);

    printf("Password : ");
    scanf(" %[^\n]", password);

    /* ইনপুট পাসওয়ার্ডকে হ্যাশে রূপান্তর */
    md5Hash(password, hash);

    while(fscanf(fp, "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]\n",
                 &u.id, u.username, u.password, u.role, u.phone) == 5) {
        if(strcasecmp(username, u.username) == 0) {
            /* রোল ভ্যালিডেশন */
            if(strcmp(role, u.role) != 0) {
                printf("\nWrong Role Login! Expected role: %s\n", role);
                fclose(fp);
                return 0;
            }

            /* পাসওয়ার্ড হ্যাশ ম্যাচিং */
            if(strcmp(hash, u.password) == 0) {
                strcpy(currentUser, u.username);
                strcpy(currentRole, u.role);
                currentUserId = u.id;

                fclose(fp);

                printf("\n=====================================\n");
                printf("        Login Successful!\n");
                printf("        Welcome %s\n", currentUser);
                printf("        Role : %s\n", currentRole);
                printf("=====================================\n");
                return 1;
            } else {
                printf("\nIncorrect Password.\n");
                fclose(fp);
                return 0;
            }
        }
    }

    fclose(fp);
    printf("\nUsername Not Found.\n");
    return 0;
}

/* ====================================================
                   AUTHENTICATION MENU
===================================================== */
int main() {
    int choice;
    initAdmin(); // ডিফল্ট অ্যাডমিন একাউন্ট নিশ্চিত করা

    while(1) {
        printf("\n=================================================\n");
        printf("           AUTHENTICATION SYSTEM MODULE          \n");
        printf("=================================================\n");

        if(strlen(currentUser) > 0) {
            printf("Logged In User : %s\n", currentUser);
            printf("Role           : %s\n", currentRole);
        } else {
            printf("Status : Not Logged In\n");
        }

        printf("-------------------------------------------------\n");
        printf("1. Customer Register\n");
        printf("2. Customer Login\n");
        printf("3. Owner Register\n");
        printf("4. Owner Login\n");
        printf("5. Admin Login\n");
        printf("0. Exit\n");
        printf("-------------------------------------------------\n");
        printf("Enter Choice : ");

        if(scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Exiting...\n");
            break;
        }

        switch(choice) {
            case 1:
                registerUser("customer");
                break;
            case 2:
                login("customer");
                break;
            case 3:
                registerUser("owner");
                break;
            case 4:
                login("owner");
                break;
            case 5:
                login("admin");
                break;
            case 0:
                printf("\nExiting Authentication Module.\n");
                exit(0);
            default:
                printf("\nInvalid Choice! Try again.\n");
        }
    }

    return 0;
}
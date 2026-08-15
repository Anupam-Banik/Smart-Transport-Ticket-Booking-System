#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#define USER_FILE       "users.txt"
#define VEHICLE_FILE    "vehicles.txt"
#define BOOKING_FILE    "bookings.txt"

#define MAX_NAME        50
#define MAX_PHONE       20
#define MAX_PASSWORD    33
#define MAX_ROLE        15
#define MAX_TYPE        20
#define MAX_DATE        20
#define MAX_PAYMENT     20
#define MAX_TRX         30
#define MAX_STATUS      20

/* ====================================================
                   STRUCTURES
===================================================== */
typedef struct {
    int id;
    char username[MAX_NAME];
    char password[MAX_PASSWORD];
    char role[MAX_ROLE];
    char phone[MAX_PHONE];
} User;

typedef struct {
    int id;
    int ownerId;
    char type[MAX_TYPE];
    char company[MAX_NAME];
    char from[MAX_NAME];
    char to[MAX_NAME];
    char departure[30];
    int totalSeats;
    int bookedSeats;
    float fare;
} Vehicle;

typedef struct {
    int bookingId;
    int vehicleId;
    char username[MAX_NAME];
    char passenger[MAX_NAME];
    char phone[MAX_PHONE];
    char journeyDate[MAX_DATE];
    int seatNo;
    float amount;
    char paymentMethod[MAX_PAYMENT];
    char trxID[MAX_TRX];
    char status[MAX_STATUS];
} Booking;

/* ====================================================
                GLOBAL SESSION VARIABLES
===================================================== */
char currentUser[MAX_NAME] = "";
char currentRole[MAX_ROLE] = "";
int currentUserId = 0;

/* ====================================================
               MD5 HASHING SUPPORT
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
            15. OWNER DASHBOARD
===================================================== */
void ownerDashboard() {
    if(strcmp(currentRole, "owner") != 0 && strcmp(currentRole, "admin") != 0) {
        printf("\nOnly Owner or Admin Access Allowed.\n");
        return;
    }

    FILE *fp = fopen(VEHICLE_FILE, "r");
    Vehicle v;

    int totalVehicle = 0;
    int totalSeats = 0;
    int totalBooked = 0;
    float totalIncome = 0;

    if(fp == NULL) {
        printf("\nNo Vehicle Found.\n");
        return;
    }

    while(fscanf(fp, "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%29[^|]|%d|%d|%f\n",
                 &v.id, &v.ownerId, v.type, v.company,
                 v.from, v.to, v.departure,
                 &v.totalSeats, &v.bookedSeats, &v.fare) == 10) {
        if(strcmp(currentRole, "admin") == 0 || v.ownerId == currentUserId) {
            totalVehicle++;
            totalSeats += v.totalSeats;
            totalBooked += v.bookedSeats;
            totalIncome += (v.bookedSeats * v.fare);
        }
    }

    fclose(fp);

    printf("\n=========================================\n");
    printf("             OWNER DASHBOARD\n");
    printf("=========================================\n");
    printf("Total Vehicle       : %d\n", totalVehicle);
    printf("Total Seat Capacity : %d\n", totalSeats);
    printf("Total Booked Seat   : %d\n", totalBooked);
    printf("Total Income        : %.2f Tk\n", totalIncome);
    printf("=========================================\n");
}

/* ====================================================
            16. ADMIN REPORT SYSTEM
===================================================== */
void adminReport() {
    if(strcmp(currentRole, "admin") != 0) {
        printf("\nOnly Admin Can Access This Report.\n");
        return;
    }

    FILE *fp;
    User u;
    Vehicle v;
    Booking b;

    int totalUser = 0;
    int totalCustomer = 0;
    int totalOwner = 0;
    int totalVehicle = 0;
    int totalBooking = 0;
    int cancelled = 0;
    float revenue = 0;

    /* USER REPORT */
    fp = fopen(USER_FILE, "r");
    if(fp != NULL) {
        while(fscanf(fp, "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]\n",
                     &u.id, u.username, u.password, u.role, u.phone) == 5) {
            totalUser++;
            if(strcmp(u.role, "customer") == 0)
                totalCustomer++;
            else if(strcmp(u.role, "owner") == 0)
                totalOwner++;
        }
        fclose(fp);
    }

    /* VEHICLE REPORT */
    fp = fopen(VEHICLE_FILE, "r");
    if(fp != NULL) {
        while(fscanf(fp, "%d|%d|%19[^|]|%49[^|]|%49[^|]|%49[^|]|%29[^|]|%d|%d|%f\n",
                     &v.id, &v.ownerId, v.type, v.company,
                     v.from, v.to, v.departure,
                     &v.totalSeats, &v.bookedSeats, &v.fare) == 10) {
            totalVehicle++;
        }
        fclose(fp);
    }

    /* BOOKING REPORT */
    fp = fopen(BOOKING_FILE, "r");
    if(fp != NULL) {
        while(fscanf(fp, "%d|%d|%49[^|]|%49[^|]|%19[^|]|%19[^|]|%d|%f|%19[^|]|%29[^|]|%19[^\n]\n",
                     &b.bookingId, &b.vehicleId, b.username, b.passenger,
                     b.phone, b.journeyDate, &b.seatNo, &b.amount,
                     b.paymentMethod, b.trxID, b.status) == 11) {
            totalBooking++;
            if(strcmp(b.status, "Confirmed") == 0) {
                revenue += b.amount;
            } else if(strcmp(b.status, "Cancelled") == 0) {
                cancelled++;
            }
        }
        fclose(fp);
    }

    printf("\n============================================\n");
    printf("              ADMIN SYSTEM REPORT\n");
    printf("============================================\n");
    printf("Total Users          : %d\n", totalUser);
    printf("Total Customers      : %d\n", totalCustomer);
    printf("Total Owners         : %d\n", totalOwner);
    printf("--------------------------------------------\n");
    printf("Total Vehicles       : %d\n", totalVehicle);
    printf("Total Bookings       : %d\n", totalBooking);
    printf("Cancelled Tickets    : %d\n", cancelled);
    printf("Total Revenue        : %.2f Tk\n", revenue);
    printf("============================================\n");
}

/* ====================================================
            17. CHANGE PASSWORD
===================================================== */
void changePassword() {
    if(strlen(currentUser) == 0) {
        printf("\nPlease Login First.\n");
        return;
    }

    FILE *fp;
    FILE *temp;
    User u;
    char oldPass[50];
    char newPass[50];
    char oldHash[33];
    char newHash[33];
    int updated = 0;

    printf("\n=====================================\n");
    printf("           CHANGE PASSWORD\n");
    printf("=====================================\n");

    printf("Old Password : ");
    scanf(" %[^\n]", oldPass);

    md5Hash(oldPass, oldHash);

    fp = fopen(USER_FILE, "r");
    temp = fopen("temp_user.txt", "w");

    if(fp == NULL || temp == NULL) {
        printf("\nFile Error.\n");
        return;
    }

    while(fscanf(fp, "%d|%49[^|]|%32[^|]|%14[^|]|%19[^\n]\n",
                 &u.id, u.username, u.password, u.role, u.phone) == 5) {
        if(strcmp(u.username, currentUser) == 0 && strcmp(u.password, oldHash) == 0) {
            printf("New Password : ");
            scanf(" %[^\n]", newPass);

            md5Hash(newPass, newHash);
            strcpy(u.password, newHash);
            updated = 1;
        }

        fprintf(temp, "%d|%s|%s|%s|%s\n", u.id, u.username, u.password, u.role, u.phone);
    }

    fclose(fp);
    fclose(temp);

    remove(USER_FILE);
    rename("temp_user.txt", USER_FILE);

    if(updated) {
        printf("\nPassword Changed Successfully.\n");
    } else {
        printf("\nOld Password Incorrect.\n");
    }
}

/* ====================================================
            18. SYSTEM REFRESH
===================================================== */
void systemRefresh() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n=====================================\n");
    printf("          SYSTEM REFRESH\n");
    printf("=====================================\n");

    initAdmin();

    printf("\nSystem Refreshed Successfully.\n");
    printf("All modules are ready.\n");
    printf("=====================================\n");
}

/* ====================================================
            19. LOGOUT
===================================================== */
void logout() {
    if(strlen(currentUser) == 0) {
        printf("\nNo User Logged In.\n");
        return;
    }

    printf("\n=====================================\n");
    printf(" User %s Logged Out Successfully.\n", currentUser);
    printf("=====================================\n");

    strcpy(currentUser, "");
    strcpy(currentRole, "");
    currentUserId = 0;
}

/* ====================================================
         STANDALONE RUNNER / MENU TEST
===================================================== */
int main() {
    int choice;
    initAdmin();

    while(1) {
        printf("\n=================================================\n");
        printf("           MANAGEMENT & SYSTEM MODULE            \n");
        printf("=================================================\n");
        if(strlen(currentUser) > 0) {
            printf("Logged In User : %s | Role: %s\n", currentUser, currentRole);
        } else {
            printf("Status : Not Logged In\n");
        }
        printf("-------------------------------------------------\n");
        printf("15. Owner Dashboard\n");
        printf("16. Admin Report\n");
        printf("17. Change Password\n");
        printf("18. System Refresh\n");
        printf("19. Logout\n");
        printf("20. Exit\n");
        printf("-------------------------------------------------\n");
        printf("Enter Choice (15-20): ");

        if(scanf("%d", &choice) != 1) {
            printf("\nInvalid input! Exiting...\n");
            break;
        }

        switch(choice) {
            case 15:
                ownerDashboard();
                break;
            case 16:
                adminReport();
                break;
            case 17:
                changePassword();
                break;
            case 18:
                systemRefresh();
                break;
            case 19:
                logout();
                break;
            case 20:
                printf("\nThank You For Using System.\n");
                exit(0);
            default:
                printf("\nInvalid Choice! Please choose between 15 and 20.\n");
        }
    }

    return 0;
}

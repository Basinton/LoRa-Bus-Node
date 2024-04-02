#ifndef AP_H
#define AP_H

#define AP_SSID_DEFAULT     "BUS-Demo"     // Define the default AP SSID
#define AP_PASSWORD_DEFAULT "12345678"     // Define the default AP password
#define ADMIN_USERNAME      "admin123456"  // Define the admin username
#define ADMIN_PASSWORD      "admin123456"  // Define the admin password
#define AP_TIMEOUT          10 * 60 * 1000 // Set timeout to 10 minutes

void AP_init();
void AP_process();

#endif // AP_H

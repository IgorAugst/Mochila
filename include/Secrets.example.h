#include <pgmspace.h>

#define SECRET

#define THINGNAME "mochila"

const char AWS_IOT_ENDPOINT[] = "a12s85dpg0xou0-ats.iot.sa-east-1.amazonaws.com";

static const char CLIENT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";

static const char CLIENT_PRIV[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";

static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
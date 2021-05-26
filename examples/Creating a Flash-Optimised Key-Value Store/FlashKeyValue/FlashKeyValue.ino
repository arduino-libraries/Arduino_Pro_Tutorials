#include <FlashIAPBlockDevice.h>
#include <TDBStore.h>

using namespace mbed;

// Get limits of the In Application Program (IAP) flash, ie. the internal MCU flash.
#include "FlashIAPLimits.h"
auto iapLimits { getFlashIAPLimits() };

// Create a block device on the available space of the FlashIAP
FlashIAPBlockDevice blockDevice(iapLimits.start_address, iapLimits.available_size);

// Create a key/value store on the Flash IAP block device
TDBStore store(&blockDevice);

// Dummy sketch stats for demonstration purposes
struct SketchStats {
    uint32_t startupTime;
    uint32_t randomValue;
    uint32_t runCount;
};

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    //  Wait for terminal to come up
    delay(1000);

    Serial.println("FlashIAPBlockDevice + TDBStore Test");

    // Feed the RNG for later content generation
    srand(micros());

    // Initialize the flash IAP block device and print the memory layout
    blockDevice.init();
    Serial.println("FlashIAP block device size: " +  blockDevice.size());
    Serial.println("FlashIAP block device read size: " + blockDevice.get_read_size());
    Serial.println("FlashIAP block device program size: " + blockDevice.get_program_size());
    Serial.println("FlashIAP block device erase size: " + blockDevice.get_erase_size());
    // Deinitialize the device
    blockDevice.deinit();

    // Initialize the key/value store
    Serial.print("Initializing TDBStore: ");
    auto result = store.init();
    Serial.println(result == MBED_SUCCESS ? "OK" : "KO");
    if (result != MBED_SUCCESS)
        while (true);

    // An example key name for the stats on the store
    const char statsKey[] { "stats" };

    // Keep track of the number of sketch executions
    uint32_t runCount { 0 };

    // Previous stats
    SketchStats previousStats;

    // Get previous run stats from the key/value store
    Serial.println("Retrieving Sketch Stats");
    result = getSketchStats(statsKey, &previousStats);
    if (result == MBED_SUCCESS) {
        Serial.println("Previous Stats");
        Serial.print("\tStartup Time: ");
        Serial.println(previousStats.startupTime);
        Serial.print("\tRandom Value: ");
        Serial.println(previousStats.randomValue);
        Serial.print("\tRun Count: ");
        Serial.println(previousStats.runCount);

        runCount = previousStats.runCount;

    } else if (result == MBED_ERROR_ITEM_NOT_FOUND) {
        Serial.println("First execution");
    } else {
        Serial.println("Error reading from key/value store.");
        while (true);
    }

    //Update the stats and save them to the store
    SketchStats currentStats { millis(), rand(), ++runCount };
    result = setSketchStats(statsKey, currentStats);
    
    if (result == MBED_SUCCESS) {
        Serial.println("Sketch Stats updated");
        Serial.println("Current Stats");
        Serial.print("\tStartup Time: ");
        Serial.println(currentStats.startupTime);
        Serial.print("\tRandom Value: ");
        Serial.println(currentStats.randomValue);
        Serial.print("\tRun Count: ");
        Serial.println(currentStats.runCount);
    } else {
        Serial.println("Error storing to key/value store");
        while (true);
    }
}

void loop()
{
    // Do nothing
}

// Retrieve a SketchStats from the k/v store
int getSketchStats(const char* key, SketchStats* stats)
{
    // Retrieve key/value info
    TDBStore::info_t info;
    auto result = store.get_info(key, &info);
    if (result == MBED_ERROR_ITEM_NOT_FOUND)
        return result;

    // Allocate space for the value
    uint8_t buffer[info.size] {};
    size_t actual_size;

    // Get the value
    result = store.get(key, buffer, sizeof(buffer), &actual_size);
    if (result != MBED_SUCCESS)
        return result;

    memcpy(stats, buffer, sizeof(SketchStats));
    return result;
}

// Store a SketchStats to the the k/v store
int setSketchStats(const char* key, SketchStats stats)
{
    auto result = store.set(key, reinterpret_cast<uint8_t*>(&stats), sizeof(SketchStats), 0);
    return result;
}

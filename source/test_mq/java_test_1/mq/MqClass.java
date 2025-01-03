package mq;

/**
 * Encapsulates interactions with a message queue (MQ) using native methods,
 * demonstrating publishing and message handling via Java Native Interface (JNI).
 */
public class MqClass {

    // Declarations of native methods for MQ interactions
    public native int init(String workRoot, int clientId, int[] topicIds);
    public native void exit();
    public native int publishTopicPollMessage(int topicId, String message, int msgSize);
    public native int publishTopicBroadcastMessage(int topicId, String message, int msgSize);
    public native int publishAddressedMessage(int clientId, int nId, int topicId, String message, int msgSize);

    // Static variables to monitor message count and MQ status
    private static int messageCount = 0;
    private static volatile boolean isMqExited = false;

    // Callbacks for MQ status updates
    public static void onReady() {
        System.out.println("Java: MQ is ready for operation.");
        for (int i = 0; i < 300; i++) {
            publishMessage();
        }
    }

    public static void onNotReady() {
        System.out.println("Java: MQ is not ready.");
    }

    public static void onExit() {
        System.out.println("Java: MQ service is exiting.");
        isMqExited = true;
    }

    // Handles messages received from MQ
    public static void onPublishAddressedMessage(int clientId, int nId, int topicId, String message, int msgSize) {
        //System.out.printf("Received message: %s [Size: %d]%n", message, msgSize);
        messageCount++;
        topicId = 5; // Specify the topic ID
        message = "123"; // Sample message to publish
        new MqClass().publishAddressedMessage(clientId, nId, topicId, message, message.length());
    }

    private static void publishMessage() {
        final int topicId = 5; // Specify the topic ID
        final String message = "123"; // Sample message to publish
        new MqClass().publishTopicPollMessage(topicId, message, message.length());
    }

    // Loads the native library on startup
    static {
        System.loadLibrary("jmq_sdk");
    }

    /**
     * Initiates the MQ test.
     *
     * @param args Not used.
     */
    public static void main(String[] args) {
        MqClass mq = new MqClass();
        String workRoot = System.getenv("WORK_ROOT");

        if (workRoot == null || workRoot.isEmpty()) {
            System.err.println("Error: WORK_ROOT environment variable is not set or is empty.");
            // Consider logging this error or handling it appropriately.
            return;
        }

        int[] topicIds = {6, 66}; // Configure topic IDs as required
        mq.init(workRoot, 1, topicIds);

        while (!isMqExited) {
            try {
                Thread.sleep(1000); // Pause to simulate asynchronous behavior
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt(); // Handle interrupted exception
                System.err.println("Thread interrupted: " + e.getMessage());
            }
            System.out.println("Message count: " + messageCount);
            messageCount = 0; // Reset the message count for the next interval
        }

        mq.exit();
        System.out.println("MQ test completed and exited successfully.");
    }
}

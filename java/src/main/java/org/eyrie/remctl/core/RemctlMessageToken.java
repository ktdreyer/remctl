package org.eyrie.remctl.core;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

import org.ietf.jgss.GSSException;

/**
 * Represents an encrypted remctl message token. This is the parent class for all remctl tokens except for the initial
 * session establishment tokens. It holds the GSS-API context that should be used to encrypt and decrypt the token and
 * supports a factory method to decrypt a token and then create the appropriate message token type.
 * 
 * 
 * <p>
 * We can't force a constructor signature on subclasses. I think we are looking for an AbstractFacotry for 'forcing' a
 * standard way to build things, but for now we will just implement the constructor on all subclasses.
 */
public abstract class RemctlMessageToken implements RemctlToken {

    /** Protocol version of token. */
    private final byte version;

    /**
     * Construct a message token with the given version. This constructor should be used by child classes to initialize
     * those fields in the token.
     * 
     * @param version
     *            The protocol version required to understand this token
     * @throws RemctlException
     *             If version is out of range. The version is not checked against {@link RemctlToken#SUPPORTED_VERSION}.
     */
    RemctlMessageToken(final int version) {
        if (version < 2 || version > 127) {
            throw new RemctlProtocolException("Invalid protocol version " + version);
        }
        this.version = (byte) version;
    }

    /**
     * Encrypt the token and write it to the provided stream.
     * 
     * @return The byte[] of the encrypted token
     * 
     * @throws IOException
     *             An error occurred writing the token to the stream
     * @throws GSSException
     *             On errors encrypting the token
     */
    @Override
    public byte[] write() throws GSSException, IOException {
        int length = this.length() + 2;
        ByteArrayOutputStream array = new ByteArrayOutputStream(length);
        DataOutputStream encode = new DataOutputStream(array);
        try {
            encode.writeByte(this.version);
            encode.writeByte(this.getType().value);
            this.writeData(encode);

            return array.toByteArray();
        } catch (IOException e) {
            // It should be impossible for writes to a ByteArrayOutputStream
            // to fail, so turn them into runtime exceptions.
            throw new RuntimeException(e);
        }

    }

    /**
     * Returns the length of the encoded token. This method is designed to be overridden by child classes of
     * <code>RemctlMessageToken</code> so that the output buffer required for a token can be sized. The fields included
     * in every token are not part of the length.
     * 
     * @return The encoded length of the token
     */
    abstract int length();

    /**
     * Writes the type-specific data to <code>DataOutputStream</code>. This method is designed to be overridden by child
     * classes of <code>RemctlMessageToken</code> so that child classes don't have to implement the rest of
     * {@link #write(OutputStream)}.
     * 
     * @param stream
     *            Output stream to which to write the token data
     * @throws IOException
     *             On error writing to the stream
     */
    abstract void writeData(DataOutputStream stream) throws IOException;

    /**
     * FIXME Do we use this? We check version in a few places. Maybe once the server is implemented
     * 
     * @return the version
     */
    private byte getVersion() {
        return this.version;
    }

    /**
     * @return the type
     */
    abstract RemctlMessageCode getType();

    /**
     * Prints the token type and version.
     */
    @Override
    public String toString() {
        return "RemctlToken: " + this.getType() + " version " + this.getVersion();
    }
}
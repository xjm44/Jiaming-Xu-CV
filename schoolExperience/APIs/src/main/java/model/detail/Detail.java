package model.detail;

import model.cryptocurrencies.Crypto;
import org.json.JSONObject;

public interface Detail {

    /**
     * <b>Preconditions:</b><br>
     *     none
     * <br>
     * <b>Postconditions:</b><br>
     *     result a cryptocurrency relates to its id.
     * <br>
     *
     * @param id cryptocurrency id.
     * @return return the cryptocurrency info according to id.
     * @throws IllegalArgumentException If any of the other preconditions are violated.
     */
    String getInfoID(String id) throws IllegalArgumentException;
    /**
     * <b>Preconditions:</b><br>
     *     none.
     * <br>
     * <b>Postconditions:</b><br>
     *     result a cryptocurrency relates to its slug.
     * <br>
     *
     * @param slug cryptocurrency slug.
     * @return return the cryptocurrency info according to slug.
     * @throws IllegalArgumentException If any of the other preconditions are violated.
     */
    String getInfoSlug(String slug)throws IllegalArgumentException;
    /**
     * <b>Preconditions:</b><br>
     *     none
     * <br>
     * <b>Postconditions:</b><br>
     *     result a cryptocurrency relates to its symbol.
     * <br>
     *
     * @param symbol cryptocurrency symbol.
     * @return return the cryptocurrency info according to symbol.
     * @throws IllegalArgumentException If any of the other preconditions are violated.
     */
    String getInfoSymbol(String symbol)throws IllegalArgumentException;

    /**
     * <b>Preconditions:</b><br>
     *     none.
     * <br>
     * <b>Postconditions:</b><br>
     *     a list of cryptocurrencies with limits and output is sorted.
     * <br>
     *
     * @param start cryptocurrency start position.
     * @param limit no. of cryptocurrencies.
     * @param sort cmc_rank/id.
     * @return return the a list of cryptocurrency info.
     * @throws IllegalArgumentException If any of the other preconditions are violated.
     */
    String getAllInfo(String start, String limit, String sort)throws IllegalArgumentException;




}



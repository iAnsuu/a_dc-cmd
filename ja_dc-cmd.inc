/**********************************
 *                                *
 *   @Author:      Ansu           *
 *   @Version:     0.0.1          *
 *   @Released:    23/03/2024     *
 *                                *
 **********************************/

#if !defined STRLIB_INC
	#error Please include <strlib> first (Download: https://github.com/oscar-broman/strlib).
#endif

#if !defined dcconnector_included
	#error Please include <discord-connector> first (Download: https://github.com/maddinat0r/samp-discord-connector/releases).
#endif

#tryinclude <strlib>
#tryinclude <discord-connector>

#define MAX_FUNC_NAME       (32 - 5)

#if !defined JA_DISCORD_PREFIX
    #define JA_DISCORD_PREFIX  '!'
#endif

#define DCMD:%0(%1,%2,%3) forward ansu_%0(%1,%2,%3); public ansu_%0(%1,%2,%3)

#if defined OnDiscordCommand
    forward OnDiscordCommand(DCC_User:user, DCC_Channel:channel, const cmdtext[], success);
#endif

public DCC_OnMessageCreate(DCC_Message:message){
    new Message_Content[255],
        DCC_Channel:channel,
        DCC_User:User,
        channelid[DCC_ID_SIZE],
        userid[DCC_ID_SIZE];
    DCC_GetMessageContent(message, Message_Content);

    if(strgetc(Message_Content, 0) != JA_DISCORD_PREFIX){
        #if defined JA_DCC_OnMessageCreate
            return JA_DCC_OnMessageCreate(DCC_Message:message);
        #else
            return true;
        #endif
    }

    DCC_GetMessageAuthor(message, User);
    DCC_GetMessageChannel(message, channel);
    
    DCC_GetChannelId(channel, channelid);
    DCC_GetUserId(User, userid);
    
    #if defined JA_ALLOW_BOT
        new bool:is_bot;
        if(!DCC_IsUserBot(User, is_bot)){
            #if defined JA_DCC_OnMessageCreate
                return JA_DCC_OnMessageCreate(DCC_Message:message);
            #else
                return true;
            #endif
        }
        if(is_bot){
            #if defined JA_DCC_OnMessageCreate
                return JA_DCC_OnMessageCreate(DCC_Message:message);
            #else
                return true;
            #endif
        }
    #endif
    strdel(Message_Content, 0, 1); //Remove  '!'

    //Find first " " ||| !command" "params
    new index = strfind(Message_Content, " ", true, 0);
    new callback[MAX_FUNC_NAME] = "ansu_";

    //No params
    if(index == -1){
        strcat(callback, Message_Content);
        index = CallLocalFunction(callback, "sss", userid, channelid, "\1");
        strdel(callback, 0, 5);
        #if defined OnDiscordCommand
            OnDiscordCommand(User, channel, callback, index);
        #endif
        #if defined JA_DCC_OnMessageCreate
            return JA_DCC_OnMessageCreate(DCC_Message:message);
        #else
            return true;
        #endif

    }

    new command[MAX_FUNC_NAME];
    strmid(command, Message_Content, 0, index);
    strcat(callback, command);
    strdel(Message_Content, 0, index + 1);

    index = CallLocalFunction(callback, "sss", userid, channelid, Message_Content);
    #if defined OnDiscordCommand
        OnDiscordCommand(User, channel, command, index);
    #endif
    #if defined JA_DCC_OnMessageCreate
        return JA_DCC_OnMessageCreate(DCC_Message:message);
    #else
        return true;
    #endif
}

#if defined _ALS_DCC_OnMessageCreate
	#undef DCC_OnMessageCreate
#else
	#define _ALS_DCC_OnMessageCreate
#endif

#define DCC_OnMessageCreate JA_DCC_OnMessageCreate

#if defined JA_DCC_OnMessageCreate
	forward JA_DCC_OnMessageCreate(DCC_Message:message);
#endif

/*
-- Use

DCMD:test(const user_id[], const channel_id[], const params[])
{
    new DCC_Channel:channel = DCC_FindChannelById(channel_id);
    new DCC_Embed:embed = DCC_CreateEmbed("Test", "Command test working", color = 0x00FF00FF, .footer_text = "Developed by: Ansu");
    DCC_SendChannelEmbedMessage(channel, embed);
}
*/

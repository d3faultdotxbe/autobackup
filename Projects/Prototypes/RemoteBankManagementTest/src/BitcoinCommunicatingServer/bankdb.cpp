#include "bankdb.h"

BankDb::BankDb(QObject *parent) :
    QObject(parent)
{
    //initialize hard-coded appId's here... will probably eventually add functionality to add these at run-time, so i don't have to recompile the server to add new clients (as i make them up out of thin air)
    QHash<QString, UserBankAccount*> *emptyListOfHashVals = new QHash<QString, UserBankAccount*>();
    m_Db.insert("lkjfosidf08043298234098234dsafljkd", emptyListOfHashVals);
}
void BankDb::addUser(const QString &appId, const QString &userName)
{
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> *listOfAccountsForThisAppId = m_Db.value(appId);
        //todo: check that this doesn't extract a hardcopy that is then modified... that we're in fact modifying the list that lives in m_Db
        if(listOfAccountsForThisAppId->contains(userName))
        {
            //todo: propagate error properly (back to client)
            emit d("user " + userName + " is already in db");
            return;
        }
        UserBankAccount *newAccount = new UserBankAccount();
        newAccount->AddFundsBitcoinKey = DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY; //TODO: our bitcoin polling loop has to check that the bitcoin key is NOT this 'default'. it can't just check every Awaiting because when the user is first created, before they try to add funds, we are in a state where they don't have a key but they ARE at AwaitingPayment. i could add another enum but it doesn't matter how you do it
        newAccount->Balance = 0.0;
        newAccount->PendingBalance = 0.0;
        newAccount->AddFundsStatus = UserBankAccount::PaymentAwaiting;

        listOfAccountsForThisAppId->insert(userName, newAccount);
        emit d("adding user '" + userName + "' to server db with appId '" + appId + "'");
    }
    else
    {
        emit d("appId is not recognized");
        return;
        //todo: propagate an error back to client saying that the appId isn't valid. already have a message in the protocol for this

        /*
        //since appId doesn't exist, we know username doesn't already exist
        QHash<QString, UserBankAccount*> newListOfAccountsForThisAppId; //new because the list doesn't yet exist since the appId doesn't yet exist
        UserBankAccount *newAccount = new UserBankAccount();
        newAccount->AddFundsBitcoinKey = DEFAULT_EMPTY_ADD_FUNDS_BITCOIN_KEY;
        newAccount->Balance = 0.0;
        newAccount->AddFundsStatus = UserBankAccount::AwaitingPayment;

        //re: TODOreq below.. this is also duplicate code that is growing... so moving it to constructor is definitely a good idea

        newlistOfAccountsForThisAppId->insert(userName, newAccount);
        m_Db.insert(appId, newListOfAccountsForThisAppId);

        //TODOreq: the appId's should not be inserted here... they should be inserted in the constructor. if we make it to this else, we should respond with the error saying INVALID_APP_ID. for now/simple-testing, we'll just add it. i'm not sure if we can have an appId with a zero size VALUE (another hash of user accounts)... and i don't care to test that right now..
        //additionally, this will NOT be in the constructor in the impl... it will be in/near like the CREATE statement for the db or some shit... (we MIGHT add the ability to add an appId without recompiling later.... but that's low priority atm)

        //i really am tempted to fix this bug... bug at the same time, not sure if the second half can be an empty QHash...
        //i wonder if since i'm passing the QHash byval that it too is modifying a COPY instead of the in-(parent)-QHash copy...
        */
    }
}
void BankDb::setAddFundsKey(const QString &appId, const QString &userName, const QString &newKey)
{
    //i just tested this and it said username not found in db (working on the fix right now)... but i noticed the key still was sent back to the client. we should check that it was actually set etc before sending it back to client. need error checking in a lot of places really...
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> *listOfAccountsForThisAppId = m_Db.value(appId);
        if(listOfAccountsForThisAppId->contains(userName))
        {
            UserBankAccount *userAccount = listOfAccountsForThisAppId->value(userName);
            userAccount->AddFundsBitcoinKey = newKey;
            userAccount->AddFundsStatus = UserBankAccount::PaymentAwaiting;
            emit d("setting addFundsKey for user: " + userName + " to: " + newKey);
        }
        else
        {
            emit d("error in setAddFundsKey, username not found in db");
        }
    }
    else
    {
        emit d("error in setAddFundsKey, appId not found in db");
    }
}
void BankDb::pendingAmountReceived(const QString &appId, const QString &username, double pendingAmount)
{
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> *listOfAccountsForThisAppId = m_Db.value(appId);
        if(listOfAccountsForThisAppId->contains(username))
        {
            UserBankAccount *userAccount = listOfAccountsForThisAppId->value(username);
            userAccount->AddFundsStatus = UserBankAccount::PaymentPending;
            userAccount->PendingBalance = pendingAmount; //todo: maybe add to the current pending amount? that's only if we change our polling logic to continue polling with 0 confirmations even after one 0 confirmations payment has been detected.. which it currently doesn't. this number doesn't even matter, so idk why we store it... it only KIND OF matters in the web facing cache db.. if the user is not logged in and then makes a payment, then logs in.. it should tell them. but since we pass it to the webclient right away, what is the point of storing it here on the server? might make shit easier in the future but i can't think of a scenario where i'd use it yet
        }
        else
        {
            emit d("error in pendingAmountReceived, username not found in db");
        }

    }
    else
    {
        emit d("error in pendingAmountReceived, appId not found in db");
    }
}
void BankDb::confirmedAmountReceived(const QString &appId, const QString &username, double confirmedAmount)
{
    if(m_Db.contains(appId))
    {
        QHash<QString, UserBankAccount*> *listOfAccountsForThisAppId = m_Db.value(appId);
        if(listOfAccountsForThisAppId->contains(username))
        {
            UserBankAccount *userAccount = listOfAccountsForThisAppId->value(username);
            userAccount->AddFundsStatus = UserBankAccount::PaymentConfirmed;
            userAccount->PendingBalance = 0.0; //see? if we zero it out here wtf was the point of storing it above?? we could check to see if the PendingBalance is the same as our new confirmedBalance.. which would really only mean that the user made 2 payments and got them both in on time. if they don't do it before the block is solved, they're fucked. we advise them of this in the GUI. there is no benefit to checking that PendingBalance is equal to our new confirmedBalance... at least i can't think of one. if there was a benefit, here would be the place
            userAccount->Balance += confirmedAmount; //we add it. if we kept a list of transactions, then this transaction's balance would be assigned to it. however, our current design is not that.
        }
        else
        {
            emit d("error in confirmedAmountReceived, username not found in db");
        }

    }
    else
    {
        emit d("error in confirmedAmountReceived, appId not found in db");
    }
}

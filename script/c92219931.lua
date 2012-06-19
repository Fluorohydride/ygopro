--二者一両損
function c92219931.initial_effect(c)
	--discard deck
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCategory(CATEGORY_DECKDES)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c92219931.discost)
	e1:SetOperation(c92219931.disop)
	c:RegisterEffect(e1)
end
function c92219931.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDiscardDeck(tp,1) and Duel.IsPlayerCanDiscardDeck(1-tp,1) end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,nil,0,PLAYER_ALL,1)
end
function c92219931.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(0,1,REASON_EFFECT)
	Duel.DiscardDeck(1,1,REASON_EFFECT)
end

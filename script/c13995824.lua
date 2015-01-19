--ボルテック・バイコーン
function c13995824.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_BEAST),aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--deckdes
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(13995824,0))
	e1:SetCategory(CATEGORY_DECKDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_DESTROYED)
	e1:SetCondition(c13995824.ddcon)
	e1:SetTarget(c13995824.ddtg)
	e1:SetOperation(c13995824.ddop)
	c:RegisterEffect(e1)
end
function c13995824.ddcon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and e:GetHandler():GetPreviousControler()==tp
end
function c13995824.ddtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,PLAYER_ALL,7)
end
function c13995824.ddop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(tp,7,REASON_EFFECT)
	Duel.DiscardDeck(1-tp,7,REASON_EFFECT)
end

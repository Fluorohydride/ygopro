--ブラッド・サッカー
function c97783659.initial_effect(c)
	--handes
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(97783659,0))
	e1:SetCategory(CATEGORY_DECKDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c97783659.ddcon)
	e1:SetTarget(c97783659.ddtg)
	e1:SetOperation(c97783659.ddop)
	c:RegisterEffect(e1)
end
function c97783659.ddcon(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c97783659.ddtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,1)
end
function c97783659.ddop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(1-tp,1,REASON_EFFECT)
end

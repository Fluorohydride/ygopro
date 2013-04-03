--黒蠍盗掘団
function c40933924.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(40933924,0))
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_DAMAGE)
	e1:SetCondition(c40933924.condition)
	e1:SetOperation(c40933924.operation)
	c:RegisterEffect(e1)
end
function c40933924.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c40933924.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(1-tp,Card.IsType,1-tp,LOCATION_DECK,0,1,1,nil,TYPE_SPELL)
	Duel.SendtoGrave(g,REASON_EFFECT)
end

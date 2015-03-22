--ボルテック・コング
function c93151201.initial_effect(c)
	--discard deck
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(93151201,0))
	e2:SetCategory(CATEGORY_DECKDES)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLE_DAMAGE)
	e2:SetCondition(c93151201.condition)
	e2:SetTarget(c93151201.target)
	e2:SetOperation(c93151201.operation)
	c:RegisterEffect(e2)
end
function c93151201.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c93151201.filter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c93151201.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local ct=Duel.GetMatchingGroupCount(c93151201.filter,tp,LOCATION_MZONE,0,nil)
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,ct)
end
function c93151201.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetMatchingGroupCount(c93151201.filter,tp,LOCATION_MZONE,0,nil)
	Duel.DiscardDeck(1-tp,ct,REASON_EFFECT)
end

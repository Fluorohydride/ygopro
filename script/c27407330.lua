--ライトレイ グレファー
function c27407330.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c27407330.spcon)
	e1:SetOperation(c27407330.spop)
	c:RegisterEffect(e1)
	--remove
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(27407330,0))
	e2:SetCategory(CATEGORY_REMOVE)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c27407330.rmcost)
	e2:SetTarget(c27407330.rmtg)
	e2:SetOperation(c27407330.rmop)
	c:RegisterEffect(e2)
end
function c27407330.spfilter(c)
	return c:GetLevel()>=5 and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c27407330.spcon(e,c)
	if c==nil then return true end
	return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0 and
		Duel.IsExistingMatchingCard(c27407330.spfilter,c:GetControler(),LOCATION_HAND,0,1,c)
end
function c27407330.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local g=Duel.SelectMatchingCard(tp,c27407330.spfilter,tp,LOCATION_HAND,0,1,1,c)
	Duel.SendtoGrave(g,REASON_DISCARD+REASON_COST)
end
function c27407330.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsDiscardable()
end
function c27407330.rmcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c27407330.costfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c27407330.costfilter,1,1,REASON_COST+REASON_DISCARD)
end
function c27407330.filter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsAbleToRemove()
end
function c27407330.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c27407330.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,LOCATION_DECK)
end
function c27407330.rmop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c27407330.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then 
		Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	end
end

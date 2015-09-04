--ナチュル・マンティス
function c51254980.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(51254980,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCost(c51254980.cost)
	e1:SetTarget(c51254980.target)
	e1:SetOperation(c51254980.operation)
	c:RegisterEffect(e1)
end
function c51254980.cfilter(c)
	return c:IsSetCard(0x2a) and c:IsType(TYPE_MONSTER) and c:IsAbleToGraveAsCost()
end
function c51254980.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c51254980.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c51254980.cfilter,tp,LOCATION_HAND,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c51254980.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return ep~=tp and eg:GetFirst():IsDestructable() end
	eg:GetFirst():CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
end
function c51254980.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end

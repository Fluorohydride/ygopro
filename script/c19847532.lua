--ヘルフレイムエンペラー
function c19847532.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(19847532,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetCondition(c19847532.condition)
	e2:SetTarget(c19847532.target)
	e2:SetOperation(c19847532.operation)
	c:RegisterEffect(e2)
end
function c19847532.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c19847532.desfilter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsDestructable()
end
function c19847532.cfilter(c)
	return c:IsAttribute(ATTRIBUTE_FIRE) and c:IsAbleToRemoveAsCost()
end
function c19847532.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c19847532.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil)
		and Duel.IsExistingMatchingCard(c19847532.cfilter,tp,LOCATION_GRAVE,0,1,nil) end
	local dg=Duel.GetMatchingGroup(c19847532.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	local ct=dg:GetCount()
	if ct>5 then ct=5 end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rg=Duel.SelectMatchingCard(tp,c19847532.cfilter,tp,LOCATION_GRAVE,0,1,ct,nil)
	Duel.Remove(rg,POS_FACEUP,REASON_COST)
	Duel.SetTargetParam(rg:GetCount())
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,dg,rg:GetCount(),0,0)
end
function c19847532.operation(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetChainInfo(0,CHAININFO_TARGET_PARAM)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectMatchingCard(tp,c19847532.desfilter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,ct,ct,nil)
	Duel.HintSelection(g)
	Duel.Destroy(g,REASON_EFFECT)
end

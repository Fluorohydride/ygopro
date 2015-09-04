--マスター・ヒュペリオン
function c55794644.initial_effect(c)
	--spsummon proc
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c55794644.hspcon)
	e1:SetOperation(c55794644.hspop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(55794644,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c55794644.condition)
	e2:SetCost(c55794644.cost)
	e2:SetTarget(c55794644.target)
	e2:SetOperation(c55794644.operation)
	c:RegisterEffect(e2)
end
function c55794644.spfilter(c)
	return c:IsSetCard(0x44) and c:IsAbleToRemoveAsCost() and (not c:IsLocation(LOCATION_MZONE) or c:IsFaceup())
end
function c55794644.hspcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local ft=Duel.GetLocationCount(tp,LOCATION_MZONE)
	if ft<=-1 then return false end
	if ft<=0 then
		return Duel.IsExistingMatchingCard(c55794644.spfilter,tp,LOCATION_MZONE,0,1,nil)
	else return Duel.IsExistingMatchingCard(c55794644.spfilter,tp,0x16,0,1,nil) end
end
function c55794644.hspop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then
		local g=Duel.SelectMatchingCard(tp,c55794644.spfilter,tp,LOCATION_MZONE,0,1,1,nil)
		Duel.Remove(g,POS_FACEUP,REASON_COST)
	else
		local g=Duel.SelectMatchingCard(tp,c55794644.spfilter,tp,0x16,0,1,1,nil)
		Duel.Remove(g,POS_FACEUP,REASON_COST)
	end
end
function c55794644.condition(e,tp,eg,ep,ev,re,r,rp)
	if Duel.IsEnvironment(56433456) then return e:GetHandler():GetFlagEffect(55794644)<2
	else return e:GetHandler():GetFlagEffect(55794644)<1 end
end
function c55794644.costfilter(c)
	return c:IsAttribute(ATTRIBUTE_LIGHT) and c:IsRace(RACE_FAIRY) and c:IsAbleToRemoveAsCost()
end
function c55794644.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c55794644.costfilter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c55794644.costfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c55794644.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsDestructable() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	e:GetHandler():RegisterFlagEffect(55794644,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end
function c55794644.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end

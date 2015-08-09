--緊急儀式術
function c38844957.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c38844957.condition)
	e1:SetCost(c38844957.cost)
	e1:SetTarget(c38844957.target)
	e1:SetOperation(c38844957.operation)
	c:RegisterEffect(e1)
end
function c38844957.cfilter(c)
	return c:IsFaceup() and bit.band(c:GetType(),0x81)==0x81
end
function c38844957.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c38844957.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c38844957.filter(c)
	return c:GetType()==TYPE_SPELL+TYPE_RITUAL and c:IsAbleToRemoveAsCost() and c:CheckActivateEffect(true,true,false)~=nil
end
function c38844957.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c38844957.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		local te=e:GetLabelObject()
		local tg=te:GetTarget()
		return tg(e,tp,eg,ep,ev,re,r,rp,0,chkc)
	end
	if chk==0 then
		if e:GetLabel()==0 then return false end
		e:SetLabel(0)
		return Duel.IsExistingMatchingCard(c38844957.filter,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,nil)
	end
	e:SetLabel(0)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c38844957.filter,tp,LOCATION_HAND+LOCATION_GRAVE,0,1,1,nil)
	local te=g:GetFirst():CheckActivateEffect(true,true,false)
	e:SetLabelObject(te)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
	e:SetCategory(te:GetCategory())
	e:SetProperty(te:GetProperty())
	local tg=te:GetTarget()
	if tg then tg(e,tp,eg,ep,ev,re,r,rp,1) end
end
function c38844957.operation(e,tp,eg,ep,ev,re,r,rp)
	local te=e:GetLabelObject()
	if not te then return end
	local op=te:GetOperation()
	if op then op(e,tp,eg,ep,ev,re,r,rp) end
end

--フェイク·フェザー
function c22628574.initial_effect(c)
	--copy trap
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0x1e1,0x1e1)
	e1:SetCost(c22628574.cost)
	e1:SetTarget(c22628574.target)
	e1:SetOperation(c22628574.operation)
	c:RegisterEffect(e1)
end
function c22628574.cfilter(c)
	return c:IsSetCard(0x33) and c:IsType(TYPE_MONSTER) and c:IsAbleToGraveAsCost()
end
function c22628574.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c22628574.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c22628574.cfilter,tp,LOCATION_HAND,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_COST)
end
function c22628574.check_effect(ce,e,tp,eg,ep,ev,re,r,rp)
	local ecode=ce:GetCode()
	if ecode~=EVENT_FREE_CHAIN and not Duel.CheckEvent(ecode) then return false end
	local con=ce:GetCondition()
	if con and not con(e,tp,eg,ep,ev,re,r,rp) then return false end
	local tg=ce:GetTarget()
	if tg and not tg(e,tp,eg,ep,ev,re,r,rp,0) then return false end
	return true
end
function c22628574.filter(c,e,tp,eg,ep,ev,re,r,rp)
	if c:GetType()~=0x4 or c:IsCode(22628574) then return false end
	local e1,e2,e3,e4,e5=c:GetActivateEffect()
	if e1 and c22628574.check_effect(e1,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e2 and c22628574.check_effect(e2,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e3 and c22628574.check_effect(e3,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e4 and c22628574.check_effect(e4,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e5 and c22628574.check_effect(e5,e,tp,eg,ep,ev,re,r,rp) then return true
	end
	return false
end
function c22628574.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then
		local te=e:GetLabelObject()
		local tg=te:GetTarget()
		return tg and tg(e,tp,eg,ep,ev,re,r,rp,1,true)
	end
	if chk==0 then return Duel.IsExistingTarget(c22628574.filter,tp,0,LOCATION_GRAVE,1,nil,e,tp,eg,ep,ev,re,r,rp) end
	e:SetProperty(EFFECT_FLAG_CARD_TARGET)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(22628574,0))
	local g=Duel.SelectTarget(tp,c22628574.filter,tp,0,LOCATION_GRAVE,1,1,nil,e,tp,eg,ep,ev,re,r,rp)
	local e1,e2,e3,e4,e5=g:GetFirst():GetActivateEffect()
	local se=nil
	if e1 and c22628574.check_effect(e1,e,tp,eg,ep,ev,re,r,rp) then se=e1
	elseif e2 and c22628574.check_effect(e2,e,tp,eg,ep,ev,re,r,rp) then se=e2
	elseif e3 and c22628574.check_effect(e3,e,tp,eg,ep,ev,re,r,rp) then se=e3
	elseif e4 and c22628574.check_effect(e4,e,tp,eg,ep,ev,re,r,rp) then se=e4
	elseif e5 and c22628574.check_effect(e5,e,tp,eg,ep,ev,re,r,rp) then se=e5
	end
	e:SetLabelObject(se)
	Duel.ClearTargetCard()
	local tg=se:GetTarget()
	e:SetCategory(se:GetCategory())
	e:SetProperty(se:GetProperty())
	if tg then tg(e,tp,eg,ep,ev,re,r,rp,1) end
end
function c22628574.operation(e,tp,eg,ep,ev,re,r,rp)
	local te=e:GetLabelObject()
	if not te then return end
	local op=te:GetOperation()
	if op then op(e,tp,eg,ep,ev,re,r,rp) end
end

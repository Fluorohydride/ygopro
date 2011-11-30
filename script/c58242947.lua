--ジャンク·コレクター
function c58242947.initial_effect(c)
	--copy trap
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(58242947,0))
	e1:SetType(EFFECT_TYPE_QUICK_O+EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,0x1e1)
	e1:SetCost(c58242947.cost)
	e1:SetTarget(c58242947.tg)
	e1:SetOperation(c58242947.op)
	c:RegisterEffect(e1)
end
function c58242947.check_effect(ce,e,tp,eg,ep,ev,re,r,rp)
	local ecode=ce:GetCode()
	if ecode~=EVENT_FREE_CHAIN and not Duel.CheckEvent(ecode) then return false end
	local con=ce:GetCondition()
	if con and not con(e,tp,eg,ep,ev,re,r,rp) then return false end
	local tg=ce:GetTarget()
	if tg and not tg(e,tp,eg,ep,ev,re,r,rp,0) then return false end
	return true
end
function c58242947.filter(c,e,tp,eg,ep,ev,re,r,rp)
	if c:GetType()~=0x4 or not c:IsAbleToRemoveAsCost() then return false end
	local e1,e2,e3,e4,e5=c:GetActivateEffect()
	if e1 and c58242947.check_effect(e1,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e2 and c58242947.check_effect(e2,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e3 and c58242947.check_effect(e3,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e4 and c58242947.check_effect(e4,e,tp,eg,ep,ev,re,r,rp) then return true
	elseif e5 and c58242947.check_effect(e5,e,tp,eg,ep,ev,re,r,rp) then return true
	end
	return false
end
function c58242947.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsAbleToRemoveAsCost()
		and Duel.IsExistingMatchingCard(c58242947.filter,tp,LOCATION_GRAVE,0,1,nil,e,tp,eg,ep,ev,re,r,rp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(58242947,1))
	local g=Duel.SelectMatchingCard(tp,c58242947.filter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp,eg,ep,ev,re,r,rp)
	local e1,e2,e3,e4,e5=g:GetFirst():GetActivateEffect()
	if e1 and c58242947.check_effect(e1,e,tp,eg,ep,ev,re,r,rp) then e:SetLabelObject(e1)
	elseif e2 and c58242947.check_effect(e2,e,tp,eg,ep,ev,re,r,rp) then e:SetLabelObject(e2)
	elseif e3 and c58242947.check_effect(e3,e,tp,eg,ep,ev,re,r,rp) then e:SetLabelObject(e3)
	elseif e4 and c58242947.check_effect(e4,e,tp,eg,ep,ev,re,r,rp) then e:SetLabelObject(e4)
	elseif e5 and c58242947.check_effect(e5,e,tp,eg,ep,ev,re,r,rp) then e:SetLabelObject(e5)
	end
	g:AddCard(c)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c58242947.tg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local te=e:GetLabelObject()
	if chkc then
		local tg=te:GetTarget()
		return tg(e,tp,eg,ep,ev,re,r,rp,0,true)
	end
	if chk==0 then return true end
	if not te then return end
	local tg=te:GetTarget()
	e:SetCategory(te:GetCategory())
	e:SetProperty(te:GetProperty())
	if tg then tg(e,tp,eg,ep,ev,re,r,rp,1) end
end
function c58242947.op(e,tp,eg,ep,ev,re,r,rp)
	local te=e:GetLabelObject()
	if not te then return end
	local op=te:GetOperation()
	if op then op(e,tp,eg,ep,ev,re,r,rp) end
end

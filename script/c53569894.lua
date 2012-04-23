--光のピラミッド
function c53569894.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_REMOVE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_LEAVE_FIELD)
	e2:SetOperation(c53569894.leave)
	c:RegisterEffect(e2)
end
function c53569894.filter(c)
	local code=c:GetCode()
	return c:IsFaceup() and (code==15013468 or code==51402177) and c:IsDestructable()
end
function c53569894.leave(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetPreviousControler()==tp and c:IsStatus(STATUS_ACTIVATED) then
		local g=Duel.GetMatchingGroup(c53569894.filter,tp,LOCATION_ONFIELD,0,nil)
		Duel.Destroy(g,REASON_EFFECT,LOCATION_REMOVED)
	end
end

--ポールポジション
function c73578229.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	local g=Group.CreateGroup()
	g:KeepAlive()
	--adjust
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EVENT_ADJUST)
	e2:SetRange(LOCATION_SZONE)
	e2:SetOperation(c73578229.adjustop)
	e2:SetLabelObject(g)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetCode(EFFECT_IMMUNE_EFFECT)
	e3:SetTarget(c73578229.etarget)
	e3:SetValue(c73578229.efilter)
	e3:SetLabelObject(g)
	c:RegisterEffect(e3)
	--destroy
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetCode(EVENT_LEAVE_FIELD)
	e4:SetOperation(c73578229.desop)
	c:RegisterEffect(e4)
end
function c73578229.etarget(e,c)
	return e:GetLabelObject():IsContains(c)
end
function c73578229.efilter(e,te)
	return te:IsActiveType(TYPE_SPELL)
end
function c73578229.adjustop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local preg=e:GetLabelObject()
	if g:GetCount()>0 then
		local ag=g:GetMaxGroup(Card.GetAttack)
		if ag:Equal(preg) then return end
		preg:Clear()
		preg:Merge(ag)
	else
		if preg:GetCount()==0 then return end
		preg:Clear()
	end
	Duel.AdjustInstantly(e:GetHandler())
	Duel.Readjust()
end
function c73578229.desop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsStatus(STATUS_ACTIVATED) then
		local g=Duel.GetMatchingGroup(Card.IsFaceup,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
		if g:GetCount()>0 then
			local ag=g:GetMaxGroup(Card.GetAttack)
			Duel.Destroy(ag,REASON_EFFECT)
		end
	end
end

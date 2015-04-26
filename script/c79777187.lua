--バリア・バブル
function c79777187.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--avoid battle damage
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c79777187.target)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetCode(EFFECT_DESTROY_REPLACE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTarget(c79777187.reptg)
	e3:SetValue(c79777187.repval)
	c:RegisterEffect(e3)
	local g=Group.CreateGroup()
	g:KeepAlive()
	e3:SetLabelObject(g)
end
function c79777187.target(e,c)
	return c:IsSetCard(0xc6) or c:IsSetCard(0x9f)
end
function c79777187.repfilter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:IsLocation(LOCATION_MZONE)
		and (c:IsSetCard(0xc6) or c:IsSetCard(0x9f)) and c:IsReason(REASON_BATTLE+REASON_EFFECT) and c:GetFlagEffect(79777187)==0
end
function c79777187.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c79777187.repfilter,1,nil,tp) end
	local g=eg:Filter(c79777187.repfilter,nil,tp)
	local tc=g:GetFirst()
	while tc do
		tc:RegisterFlagEffect(79777187,RESET_EVENT+0x1fc0000+RESET_PHASE+RESET_END,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(79777187,0))
		tc=g:GetNext()
	end
	e:GetLabelObject():Clear()
	e:GetLabelObject():Merge(g)
	return true
end
function c79777187.repval(e,c)
	local g=e:GetLabelObject()
	return g:IsContains(c)
end

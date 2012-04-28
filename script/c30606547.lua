--暗黒の扉
function c30606547.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--cannot attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetCondition(c30606547.atkcon)
	e2:SetTarget(c30606547.atktg)
	c:RegisterEffect(e2)
	--check
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetOperation(c30606547.checkop)
	e3:SetLabelObject(e2)
	c:RegisterEffect(e3)
end
function c30606547.atkcon(e)
	return e:GetHandler():GetFlagEffect(30606547)~=0
end
function c30606547.atktg(e,c)
	return c:GetFieldID()~=e:GetLabel()
end
function c30606547.checkop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():GetFlagEffect(30606547)~=0 then return end
	local fid=eg:GetFirst():GetFieldID()
	e:GetHandler():RegisterFlagEffect(30606547,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	e:GetLabelObject():SetLabel(fid)
end

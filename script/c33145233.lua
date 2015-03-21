--儀式魔人デモリッシャー
function c33145233.initial_effect(c)
	--ritual material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_RITUAL_MATERIAL)
	c:RegisterEffect(e1)
	--become material
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e2:SetCode(EVENT_BE_MATERIAL)
	e2:SetCondition(c33145233.condition)
	e2:SetOperation(c33145233.operation)
	c:RegisterEffect(e2)
end
function c33145233.condition(e,tp,eg,ep,ev,re,r,rp)
	return r==REASON_RITUAL
end
function c33145233.operation(e,tp,eg,ep,ev,re,r,rp)
	local rc=e:GetHandler():GetReasonCard()
	if rc:GetFlagEffect(33145233)==0 then
		--untargetable
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetDescription(aux.Stringid(33145233,0))
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_CLIENT_HINT)
		e1:SetRange(LOCATION_MZONE)
		e1:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
		e1:SetValue(aux.tgoval)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		rc:RegisterEffect(e1,true)
		rc:RegisterFlagEffect(33145233,RESET_EVENT+0x1fe0000,0,1)
	end
end

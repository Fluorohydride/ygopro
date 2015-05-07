--ナチュル・ビートル
function c27762803.initial_effect(c)
	--ad change
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_CHAIN_SOLVED)
	e1:SetOperation(c27762803.adop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetCode(EFFECT_SET_BASE_ATTACK)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c27762803.con)
	e2:SetValue(1800)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_SET_BASE_DEFENCE)
	e3:SetValue(400)
	c:RegisterEffect(e3)
end
function c27762803.con(e)
	return e:GetHandler():GetFlagEffect(27762803)~=0
end
function c27762803.adop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) then
		if e:GetHandler():GetFlagEffect(27762803)==0 then
			e:GetHandler():RegisterFlagEffect(27762803,RESET_EVENT+0x1ff0000,0,0)
		else
			e:GetHandler():ResetFlagEffect(27762803)
		end
	end
end

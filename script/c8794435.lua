--合成魔獣 ガーゼット
function c8794435.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c8794435.atop)
	c:RegisterEffect(e1)
	--tribute check
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MATERIAL_CHECK)
	e2:SetValue(c8794435.valcheck)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c8794435.valcheck(e,c)
	local g=c:GetMaterial()
	local tc=g:GetFirst()
	local atk=0
	while tc do
		local catk=tc:GetTextAttack()
		atk=atk+(catk>=0 and catk or 0)
		tc=g:GetNext()
	end
	e:GetLabelObject():SetLabel(atk)
end
function c8794435.atop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(e:GetLabel())
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
end

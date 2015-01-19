--ストロング・ウィンド・ドラゴン
function c23770284.initial_effect(c)
	--summon success
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(23770284,0))
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetCondition(c23770284.condition)
	e1:SetOperation(c23770284.operation)
	c:RegisterEffect(e1)
	--tribute check
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MATERIAL_CHECK)
	e2:SetValue(c23770284.valcheck)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	--indes
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e3:SetValue(c23770284.indval)
	c:RegisterEffect(e3)
	--pierce
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_PIERCE)
	c:RegisterEffect(e4)
end
function c23770284.valcheck(e,c)
	local g=c:GetMaterial()
	local tc=g:GetFirst()
	if tc:IsRace(RACE_DRAGON) then
		e:GetLabelObject():SetLabel(1)
	else
		e:GetLabelObject():SetLabel(0)
	end
end
function c23770284.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE and e:GetLabel()==1
end
function c23770284.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsRelateToEffect(e) then
		local atk=c:GetMaterial():GetFirst():GetTextAttack()
		if atk<0 then atk=0 end
		if atk>0 then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_ATTACK)
			e1:SetValue(atk/2)
			e1:SetReset(RESET_EVENT+0x1ff0000)
			c:RegisterEffect(e1)
		end
	end
end
function c23770284.indval(e,c)
	return c:GetAttack()==e:GetHandler():GetAttack()
end

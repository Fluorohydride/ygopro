--グリード・クエーサー
function c50263751.initial_effect(c)
	--base attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_BASE_ATTACK)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c50263751.atkval)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_SET_BASE_DEFENCE)
	c:RegisterEffect(e2)
	--lvup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetCondition(c50263751.condition)
	e2:SetOperation(c50263751.operation)
	c:RegisterEffect(e2)
end
function c50263751.atkval(e,c)
	return c:GetLevel()*300
end
function c50263751.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsRelateToBattle() and e:GetHandler():IsFaceup()
end
function c50263751.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local bc=c:GetBattleTarget()
	local lv=bc:GetLevel()
	if lv>0 then
		if c:GetFlagEffect(50263751)==0 then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_UPDATE_LEVEL)
			e1:SetValue(lv)
			e1:SetReset(RESET_EVENT+0x1ff0000)
			c:RegisterEffect(e1)
			c:RegisterFlagEffect(50263751,RESET_EVENT+0x1ff0000,0,0)
			e:SetLabelObject(e1)
			e:SetLabel(lv)
		else
			local pe=e:GetLabelObject()
			local ct=e:GetLabel()+lv
			e:SetLabel(ct)
			pe:SetValue(ct)
		end
	end
end

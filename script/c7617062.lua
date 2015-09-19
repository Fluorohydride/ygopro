--ゴーストリック・ミュージアム
function c7617062.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--cannot attack
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_ATTACK)
	e2:SetRange(LOCATION_FZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c7617062.ftarget)
	c:RegisterEffect(e2)
	--atklimit
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_SELECT_BATTLE_TARGET)
	e3:SetRange(LOCATION_FZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetValue(c7617062.bttg)
	c:RegisterEffect(e3)
	--direct attack
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_DIRECT_ATTACK)
	e4:SetRange(LOCATION_FZONE)
	e4:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e4:SetTarget(c7617062.dirtg)
	c:RegisterEffect(e4)
	--pos
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e5:SetRange(LOCATION_FZONE)
	e5:SetCode(EVENT_BATTLE_DAMAGE)
	e5:SetOperation(c7617062.regop)
	c:RegisterEffect(e5)
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e6:SetRange(LOCATION_FZONE)
	e6:SetCode(EVENT_DAMAGE_STEP_END)
	e6:SetCondition(c7617062.poscon)
	e6:SetOperation(c7617062.posop)
	e6:SetLabelObject(e5)
	c:RegisterEffect(e6)
end
function c7617062.bttg(e,c)
	return c:IsFacedown()
end
function c7617062.dirtg(e,c)
	return not Duel.IsExistingMatchingCard(Card.IsFaceup,c:GetControler(),0,LOCATION_MZONE,1,nil)
end
function c7617062.ftarget(e,c)
	return not c:IsSetCard(0x8d)
end
function c7617062.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	c:RegisterFlagEffect(7617062,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE,0,1)
	eg:GetFirst():RegisterFlagEffect(7617063,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE,0,1)
end
function c7617062.poscon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(7617062)>0
end
function c7617062.filter(c)
	return c:GetFlagEffect(7617063)>0
end
function c7617062.posop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c7617062.filter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.ChangePosition(g,POS_FACEDOWN_DEFENCE)
	end
end

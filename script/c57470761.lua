--タイラント・ウィング
function c57470761.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCondition(c57470761.condition)
	e1:SetTarget(c57470761.target)
	e1:SetOperation(c57470761.activate)
	c:RegisterEffect(e1)
end
function c57470761.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c57470761.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_DRAGON)
end
function c57470761.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c57470761.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c57470761.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	Duel.SelectTarget(tp,c57470761.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c57470761.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsLocation(LOCATION_SZONE) then return end
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e) and tc:IsFaceup() then
		Duel.Equip(tp,c,tc)
		c:CancelToGrave()
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EQUIP_LIMIT)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetValue(c57470761.eqlimit)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_EQUIP)
		e2:SetCode(EFFECT_UPDATE_ATTACK)
		e2:SetValue(400)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e2)
		local e3=e2:Clone()
		e3:SetCode(EFFECT_UPDATE_DEFENCE)
		c:RegisterEffect(e3)
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_EQUIP)
		e4:SetCode(EFFECT_EXTRA_ATTACK)
		e4:SetValue(1)
		e4:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e4)
		local e5=Effect.CreateEffect(c)
		e5:SetType(EFFECT_TYPE_EQUIP)
		e5:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
		e5:SetCondition(c57470761.dircon)
		e5:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e5)
		local e6=e5:Clone()
		e6:SetCode(EFFECT_CANNOT_ATTACK)
		e6:SetCondition(c57470761.atkcon)
		c:RegisterEffect(e6)
		local e7=Effect.CreateEffect(c)
		e7:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e7:SetCode(EVENT_BATTLE_START)
		e7:SetRange(LOCATION_SZONE)
		e7:SetOperation(c57470761.regop)
		e7:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e7)
		local e8=Effect.CreateEffect(c)
		e8:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e8:SetCode(EVENT_EQUIP)
		e8:SetRange(LOCATION_SZONE)
		e8:SetOperation(c57470761.resetop)
		e8:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e8)
		local e9=Effect.CreateEffect(c)
		e9:SetCategory(CATEGORY_DESTROY)
		e9:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
		e9:SetCode(EVENT_PHASE+PHASE_END)
		e9:SetRange(LOCATION_SZONE)
		e9:SetCountLimit(1)
		e9:SetCondition(c57470761.descon)
		e9:SetTarget(c57470761.destg)
		e9:SetOperation(c57470761.desop)
		e9:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e9)
	end
end
function c57470761.eqlimit(e,c)
	return c:IsRace(RACE_DRAGON)
end
function c57470761.dircon(e)
	return e:GetHandler():GetEquipTarget():GetAttackAnnouncedCount()>0
end
function c57470761.atkcon(e)
	return e:GetHandler():GetEquipTarget():IsDirectAttacked()
end
function c57470761.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ec=c:GetEquipTarget()
	if not ec:IsRelateToBattle() then return end
	local bc=ec:GetBattleTarget()
	if bc and bc:IsControler(1-tp) then
		c:RegisterFlagEffect(57470761,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
	end
end
function c57470761.resetop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if eg:IsContains(c) then
		c:ResetFlagEffect(57470761)
	end
end
function c57470761.descon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetFlagEffect(57470761)~=0
end
function c57470761.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetHandler(),1,0,0)
end
function c57470761.desop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end

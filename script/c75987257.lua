--隷属の鱗粉
function c75987257.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION+CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c75987257.condition)
	e1:SetTarget(c75987257.target)
	e1:SetOperation(c75987257.operation)
	c:RegisterEffect(e1)
	--pos
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(75987257,0))
	e2:SetType(EFFECT_TYPE_QUICK_O)
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c75987257.poscon)
	e2:SetOperation(c75987257.posop)
	c:RegisterEffect(e2)
end
function c75987257.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp
end
function c75987257.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local tg=Duel.GetAttacker()
	if chkc then return chkc==tg end
	if chk==0 then return tg:IsOnField() and tg:IsDestructable() and tg:IsCanBeEffectTarget(e) end
	Duel.SetTargetCard(tg)
end
function c75987257.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsLocation(LOCATION_SZONE) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsAttackable() and not tc:IsStatus(STATUS_ATTACK_CANCELED) then
		Duel.ChangePosition(tc,POS_FACEUP_DEFENCE)
		if c:IsRelateToEffect(e) then
			Duel.Equip(tp,c,tc)
			c:CancelToGrave()
			--Equip limit
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_EQUIP_LIMIT)
			e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			e1:SetValue(c75987257.eqlimit)
			e1:SetLabelObject(tc)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			c:RegisterEffect(e1)
		end
	end
end
function c75987257.eqlimit(e,c)
	return c==e:GetLabelObject()
end
function c75987257.poscon(e,tp,eg,ep,ev,re,r,rp)
	local ph=Duel.GetCurrentPhase()
	return (ph==PHASE_MAIN1 or ph==PHASE_BATTLE or ph==PHASE_MAIN2) and e:GetHandler():GetEquipTarget()
end
function c75987257.posop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local ec=c:GetEquipTarget()
	if ec then
		Duel.ChangePosition(ec,POS_FACEUP_DEFENCE,0,POS_FACEUP_ATTACK,0)
	end
end

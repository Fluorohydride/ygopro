--地縛神の咆哮
function c56339050.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c56339050.target)
	e1:SetOperation(c56339050.operation)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(56339050,0))
	e2:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_ATTACK_ANNOUNCE)
	e2:SetCondition(c56339050.condition2)
	e2:SetTarget(c56339050.target2)
	e2:SetOperation(c56339050.operation)
	c:RegisterEffect(e2)
end
function c56339050.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc==Duel.GetAttacker() end
	if chk==0 then return true end
	if Duel.CheckEvent(EVENT_ATTACK_ANNOUNCE) and Duel.GetTurnPlayer()~=tp then
		local tc=Duel.GetAttacker()
		if Duel.IsExistingMatchingCard(c56339050.cfilter,tp,LOCATION_MZONE,0,1,nil,tc:GetAttack())
			and tc:IsOnField() and tc:IsCanBeEffectTarget(e) and tc:IsDestructable() then
			e:SetCategory(CATEGORY_DESTROY+CATEGORY_DAMAGE)
			e:SetProperty(EFFECT_FLAG_CARD_TARGET)
			Duel.SetTargetCard(tc)
			local dam=tc:GetAttack()/2
			Duel.SetTargetParam(dam)
			Duel.SetOperationInfo(0,CATEGORY_DESTROY,tc,1,0,0)
			Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
			e:GetHandler():RegisterFlagEffect(56339050,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
		else
			e:SetCategory(0)
			e:SetProperty(0)
		end
	end
end
function c56339050.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsRelateToEffect(e) and tc:IsFaceup() and tc:IsAttackable() then
		local atk=tc:GetAttack()/2
		if Duel.Destroy(tc,REASON_EFFECT)~=0 then
			Duel.Damage(1-tp,atk,REASON_EFFECT)
		end
	end
end
function c56339050.cfilter(c,atk)
	return c:IsFaceup() and c:IsSetCard(0x21) and c:GetAttack()>atk
end
function c56339050.condition2(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttacker()
	return Duel.GetTurnPlayer()~=tp and Duel.IsExistingMatchingCard(c56339050.cfilter,tp,LOCATION_MZONE,0,1,nil,tc:GetAttack())
end
function c56339050.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local tc=Duel.GetAttacker()
	if chkc then return chkc==tc end
	if chk==0 then return e:GetHandler():GetFlagEffect(56339050)==0
		and tc:IsOnField() and tc:IsCanBeEffectTarget(e) and tc:IsDestructable() end
	Duel.SetTargetCard(tc)
	local dam=tc:GetAttack()/2
	Duel.SetTargetParam(dam)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,tc,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,dam)
	e:GetHandler():RegisterFlagEffect(56339050,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
end

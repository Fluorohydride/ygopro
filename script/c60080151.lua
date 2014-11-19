--好敵手の記憶
function c60080151.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE+CATEGORY_REMOVE+CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c60080151.condition)
	e1:SetTarget(c60080151.target)
	e1:SetOperation(c60080151.activate)
	c:RegisterEffect(e1)
end
function c60080151.condition(e,tp,eg,ep,ev,re,r,rp)
	return tp~=Duel.GetTurnPlayer()
end
function c60080151.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local tg=Duel.GetAttacker()
	if chk==0 then return tg:IsOnField() and tg:IsAbleToRemove() end
	local dam=tg:GetAttack()
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,tp,dam)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,tg,1,0,0)
end
function c60080151.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetAttacker()
	if tc and tc:IsAttackable() and not tc:IsStatus(STATUS_ATTACK_CANCELED) then
		local dam=tc:GetAttack()
		if Duel.Damage(tp,dam,REASON_EFFECT)>0 and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)>0 then
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
			e1:SetCountLimit(1)
			e1:SetCode(EVENT_PHASE+PHASE_END)
			e1:SetCondition(c60080151.spcon)
			e1:SetOperation(c60080151.spop)
			e1:SetLabelObject(tc)
			e1:SetLabel(Duel.GetTurnCount())
			e1:SetReset(RESET_PHASE+PHASE_END+RESET_OPPO_TURN,2)
			Duel.RegisterEffect(e1,tp)
			tc:RegisterFlagEffect(60080151,RESET_EVENT+0x1fe0000,0,0)
		end
	end
end
function c60080151.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	return Duel.GetTurnCount()~=e:GetLabel() and Duel.GetTurnPlayer()~=tp
		and tc:GetFlagEffect(60080151)~=0 and tc:GetReasonEffect():GetHandler()==e:GetHandler()
end
function c60080151.spop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SpecialSummon(e:GetLabelObject(),0,tp,tp,false,false,POS_FACEUP)
end

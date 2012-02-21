--ガムシャラ
function c85709845.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetTarget(c85709845.target)
	e1:SetOperation(c85709845.activate)
	c:RegisterEffect(e1)
end
function c85709845.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local d=Duel.GetAttackTarget()
	if chk==0 then return d:IsDefence() and d:IsControler(tp) end
	Duel.SetTargetCard(d)
end
function c85709845.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsDefence() then
		Duel.ChangePosition(tc,POS_FACEUP_ATTACK)
		local a=Duel.GetAttacker()
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
		e1:SetCode(EVENT_BATTLE_DESTROYED)
		e1:SetOperation(c85709845.damop)
		e1:SetLabel(1-tp)
		e1:SetReset(RESET_EVENT+0x17a0000+RESET_PHASE+PHASE_DAMAGE)
		a:RegisterEffect(e1)
	end
end
function c85709845.damop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local atk=c:GetAttack()
	if atk<0 then atk=0 end
	if atk>0 then
		Duel.Hint(HINT_CARD,tp,99004752)
		Duel.Hint(HINT_CARD,1-tp,99004752)
		Duel.Damage(e:GetLabel(),atk,REASON_EFFECT)
	end
end

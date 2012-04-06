--念導力
function c23323812.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c23323812.condition)
	e1:SetTarget(c23323812.target)
	e1:SetOperation(c23323812.activate)
	c:RegisterEffect(e1)
end
function c23323812.filter(c,tp)
	return c:GetPreviousControler()==tp and bit.band(c:GetBattlePosition(),POS_FACEUP)~=0
		and c==Duel.GetAttackTarget() and c:IsRace(RACE_PSYCHO)
end
function c23323812.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c23323812.filter,1,nil,tp)
end
function c23323812.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetAttacker():IsRelateToBattle() end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttacker(),1,0,0)
end
function c23323812.activate(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	if a:IsRelateToBattle() then
		local atk=a:GetAttack()
		if Duel.Destroy(a,REASON_EFFECT)~=0 then
			Duel.Recover(tp,atk,REASON_EFFECT)
		end
	end
end

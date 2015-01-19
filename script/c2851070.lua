--魔鏡導士リフレクト・バウンダー
function c2851070.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(2851070,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_CONFIRM)
	e1:SetCondition(c2851070.damcon)
	e1:SetTarget(c2851070.damtg)
	e1:SetOperation(c2851070.damop)
	c:RegisterEffect(e1)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(2851070,1))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLED)
	e1:SetTarget(c2851070.destg)
	e1:SetOperation(c2851070.desop)
	c:RegisterEffect(e1)
end
function c2851070.damcon(e,tp,eg,ep,ev,re,r,rp)
	local c=Duel.GetAttackTarget()
	return c==e:GetHandler() and c:GetBattlePosition()==POS_FACEUP_ATTACK
end
function c2851070.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk ==0 then	return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
end
function c2851070.damop(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local lp1=Duel.GetLP(p)
	Duel.Damage(p,Duel.GetAttacker():GetAttack(),REASON_EFFECT)
	local lp2=Duel.GetLP(p)
	if lp2<lp1 then
		e:GetHandler():RegisterFlagEffect(2851070,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_DAMAGE,0,1)
	end
end
function c2851070.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetFlagEffect(2851070)~=0 end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetHandler(),1,0,0)
end
function c2851070.desop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.Destroy(e:GetHandler(),REASON_EFFECT)
	end
end

--機械犬マロン
function c94667532.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(94667532,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c94667532.damcon)
	e1:SetTarget(c94667532.damtg)
	e1:SetOperation(c94667532.damop)
	c:RegisterEffect(e1)
end
function c94667532.damcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD) and e:GetHandler():IsReason(REASON_DESTROY)
end
function c94667532.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local p=PLAYER_ALL
	if not e:GetHandler():IsReason(REASON_BATTLE) then
		p=1-tp
	end
	Duel.SetTargetPlayer(p)
	Duel.SetTargetParam(1000)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,p,1000)
end
function c94667532.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	if p==PLAYER_ALL then
		Duel.Damage(0,d,REASON_EFFECT)
		Duel.Damage(1,d,REASON_EFFECT)
	else
		Duel.Damage(p,d,REASON_EFFECT)
	end
end

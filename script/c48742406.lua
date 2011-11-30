--フェアリー·アーチャー
function c48742406.initial_effect(c)
	--damage
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(48742406,0))
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c48742406.damtg)
	e1:SetOperation(c48742406.damop)
	c:RegisterEffect(e1)
end
function c48742406.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFlagEffect(tp,48742406)==0 end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
	Duel.RegisterFlagEffect(tp,48742406,RESET_PHASE+PHASE_END,0,1)
end
function c48742406.dfilter(c)
	return c:IsFaceup() and c:IsAttribute(ATTRIBUTE_LIGHT)
end
function c48742406.damop(e,tp,eg,ep,ev,re,r,rp)
	local p=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER)
	local ct=Duel.GetMatchingGroupCount(c48742406.dfilter,tp,LOCATION_MZONE,0,nil)
	Duel.Damage(p,ct*400,REASON_EFFECT)
end

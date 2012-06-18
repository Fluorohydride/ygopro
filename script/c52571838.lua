--忍び寄るデビルマンタ
function c52571838.initial_effect(c)
	--summon success
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetOperation(c52571838.sumsuc)
	c:RegisterEffect(e1)
end
function c52571838.sumsuc(e,tp,eg,ep,ev,re,r,rp)
	Duel.SetChainLimitTillChainEnd(c52571838.chlimit)
end
function c52571838.chlimit(re,rp,tp)
	return not re:GetHandler():IsType(TYPE_TRAP) or not re:IsHasType(EFFECT_TYPE_ACTIVATE)
end

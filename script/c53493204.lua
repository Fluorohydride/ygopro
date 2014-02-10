--心眼の女神
function c53493204.initial_effect(c)
	--fusion substitute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_FUSION_SUBSTITUTE)
	e1:SetCondition(c53493204.subcon)
	c:RegisterEffect(e1)
end
function c53493204.subcon(e)
	return e:GetHandler():IsLocation(LOCATION_HAND+LOCATION_ONFIELD+LOCATION_GRAVE)
end
